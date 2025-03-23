/**
 * Asian Cryptocurrency Payment System - Kiosk SDK Implementation
 * Version: 1.0.0
 * 
 * This file contains the implementation of the Asian Cryptocurrency Payment System
 * for self-service kiosks across Asian markets.
 */

#include "asian_crypto_payment.h"

namespace AsianCryptoPay {

AsianCryptoPayment::AsianCryptoPayment(const QString& apiKey, const QString& merchantId, CountryCode countryCode, QObject* parent)
    : QObject(parent)
    , m_apiKey(apiKey)
    , m_merchantId(merchantId)
    , m_countryCode(countryCode)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_countryModule(createCountryModule(countryCode))
    , m_securityModule(std::make_unique<SecurityModule>(apiKey))
{
    // Default supported cryptocurrencies
    m_supportedCryptocurrencies << "BTC" << "ETH" << "USDT" << "USDC" << "BNB";
    
    // Connect network manager
    connect(m_networkManager, &QNetworkAccessManager::finished, this, &AsianCryptoPayment::onNetworkReply);
    
    qDebug() << "SDK initialized for country:" << m_countryModule->countryName();
}

AsianCryptoPayment::~AsianCryptoPayment() {
    // Stop all payment timers
    for (auto timer : m_paymentTimers.values()) {
        timer->stop();
        timer->deleteLater();
    }
    m_paymentTimers.clear();
}

void AsianCryptoPayment::setTestMode(bool testMode) {
    m_testMode = testMode;
}

void AsianCryptoPayment::setApiEndpoint(const QString& apiEndpoint) {
    m_apiEndpoint = apiEndpoint;
}

void AsianCryptoPayment::setSupportedCryptocurrencies(const QStringList& supportedCryptocurrencies) {
    m_supportedCryptocurrencies = supportedCryptocurrencies;
}

void AsianCryptoPayment::setWebhookConfig(const QString& webhookEndpoint, const QString& webhookSecret) {
    m_webhookConfig["endpoint"] = webhookEndpoint;
    m_webhookConfig["secret"] = webhookSecret;
}

void AsianCryptoPayment::createPayment(const PaymentDetails& paymentDetails) {
    try {
        // Validate payment details
        validatePaymentDetails(paymentDetails);
        
        // Apply country-specific validations
        m_countryModule->validatePayment(paymentDetails);
        
        // Prepare payment data
        QJsonObject paymentData = paymentDetails.toJson();
        paymentData["merchant_id"] = m_merchantId;
        paymentData["country_code"] = countryCodeToString(m_countryCode);
        paymentData["test_mode"] = m_testMode;
        
        // Make API request
        makeApiRequest("payments", "POST", paymentData);
    } catch (const std::exception& e) {
        emit error(400, QString::fromStdString(e.what()));
    }
}

void AsianCryptoPayment::getPayment(const QString& paymentId) {
    if (paymentId.isEmpty()) {
        emit error(400, "Payment ID is required");
        return;
    }
    
    QString endpoint = "payments/" + paymentId;
    makeApiRequest(endpoint, "GET");
}

void AsianCryptoPayment::getPayments(const PaymentFilters& filters) {
    QString endpoint = "payments";
    
    // Add query parameters if filters are provided
    QString queryString = filters.buildQueryString();
    if (!queryString.isEmpty()) {
        endpoint += "?" + queryString;
    }
    
    makeApiRequest(endpoint, "GET");
}

void AsianCryptoPayment::cancelPayment(const QString& paymentId) {
    if (paymentId.isEmpty()) {
        emit error(400, "Payment ID is required");
        return;
    }
    
    QString endpoint = "payments/" + paymentId + "/cancel";
    makeApiRequest(endpoint, "POST");
}

void AsianCryptoPayment::getExchangeRates(const QString& baseCurrency, const QStringList& cryptoCurrencies) {
    if (baseCurrency.isEmpty()) {
        emit error(400, "Base currency is required");
        return;
    }
    
    QStringList currencies = cryptoCurrencies.isEmpty() ? m_supportedCryptocurrencies : cryptoCurrencies;
    
    QString endpoint = "exchange-rates?base_currency=" + baseCurrency + "&currencies=" + currencies.join(",");
    makeApiRequest(endpoint, "GET");
}

bool AsianCryptoPayment::verifyWebhookSignature(const QString& signature, const QString& body) {
    if (!m_webhookConfig.contains("secret")) {
        qWarning() << "Webhooks not initialized";
        return false;
    }
    
    QString secret = m_webhookConfig["secret"].toString();
    return m_securityModule->verifySignature(signature, body, secret);
}

bool AsianCryptoPayment::processWebhookEvent(const QJsonObject& event, const QString& signature) {
    if (m_webhookConfig.isEmpty()) {
        qWarning() << "Webhooks not initialized";
        return false;
    }
    
    // Verify signature
    QJsonDocument doc(event);
    QString eventString = doc.toJson(QJsonDocument::Compact);
    bool isValid = verifyWebhookSignature(signature, eventString);
    
    if (!isValid) {
        qWarning() << "Invalid webhook signature";
        return false;
    }
    
    // Process event
    try {
        QString eventType = event["type"].toString();
        
        if (event.contains("data") && event["data"].isObject()) {
            Payment payment = Payment::fromJson(event["data"].toObject());
            
            if (eventType == "payment.created") {
                emit paymentCreated(payment);
            } else if (eventType == "payment.updated") {
                emit paymentStatusUpdated(payment);
            } else if (eventType == "payment.completed") {
                emit paymentStatusUpdated(payment);
                stopPaymentStatusCheck(payment.id());
            } else if (eventType == "payment.cancelled") {
                emit paymentStatusUpdated(payment);
                stopPaymentStatusCheck(payment.id());
            } else if (eventType == "payment.expired") {
                emit paymentStatusUpdated(payment);
                stopPaymentStatusCheck(payment.id());
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        qWarning() << "Failed to process webhook event:" << e.what();
        return false;
    }
}

void AsianCryptoPayment::downloadQrCode(const QString& url) {
    if (url.isEmpty()) {
        emit error(400, "QR code URL is required");
        return;
    }
    
    QNetworkRequest request(url);
    QNetworkReply* reply = m_networkManager->get(request);
    
    RequestContext context;
    context.type = RequestType::DownloadQrCode;
    m_pendingRequests[reply] = context;
    
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onQrCodeDownloaded(reply);
    });
}

void AsianCryptoPayment::validatePaymentDetails(const PaymentDetails& paymentDetails) {
    if (paymentDetails.amount() <= 0.0) {
        throw std::invalid_argument("Payment amount must be greater than zero");
    }
    
    if (paymentDetails.currency().isEmpty()) {
        throw std::invalid_argument("Currency is required");
    }
    
    if (paymentDetails.cryptoCurrency().isEmpty()) {
        throw std::invalid_argument("Cryptocurrency is required");
    }
    
    if (!m_supportedCryptocurrencies.contains(paymentDetails.cryptoCurrency())) {
        throw std::invalid_argument("Unsupported cryptocurrency. Must be one of: " + 
                m_supportedCryptocurrencies.join(", ").toStdString());
    }
}

QNetworkRequest AsianCryptoPayment::createApiRequest(const QString& endpoint, const QJsonObject& data) {
    QString url = m_apiEndpoint + "/" + endpoint;
    QNetworkRequest request(url);
    
    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
    
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-Merchant-ID", m_merchantId.toUtf8());
    request.setRawHeader("X-Timestamp", timestamp.toUtf8());
    request.setRawHeader("X-Test-Mode", m_testMode ? "true" : "false");
    request.setRawHeader("User-Agent", "AsianCryptoPayment-Qt/1.0.0");
    
    if (!data.isEmpty()) {
        QJsonDocument doc(data);
        QString dataString = doc.toJson(QJsonDocument::Compact);
        QString signature = m_securityModule->generateSignature(dataString, timestamp);
        request.setRawHeader("X-Signature", signature.toUtf8());
    }
    
    return request;
}

void AsianCryptoPayment::makeApiRequest(const QString& endpoint, const QString& method, const QJsonObject& data) {
    QNetworkRequest request = createApiRequest(endpoint, data);
    QNetworkReply* reply = nullptr;
    
    if (method == "GET") {
        reply = m_networkManager->get(request);
    } else if (method == "POST") {
        QJsonDocument doc(data);
        QByteArray requestData = doc.toJson(QJsonDocument::Compact);
        reply = m_networkManager->post(request, requestData);
    } else if (method == "PUT") {
        QJsonDocument doc(data);
        QByteArray requestData = doc.toJson(QJsonDocument::Compact);
        reply = m_networkManager->put(request, requestData);
    } else if (method == "DELETE") {
        reply = m_networkManager->deleteResource(request);
    }
    
    if (reply) {
        RequestContext context;
        
        if (endpoint.startsWith("payments") && method == "POST" && !endpoint.contains("/cancel")) {
            context.type = RequestType::CreatePayment;
        } else if (endpoint.startsWith("payments/") && method == "GET") {
            context.type = RequestType::GetPayment;
            context.id = endpoint.mid(9);
        } else if (endpoint.startsWith("payments") && method == "GET") {
            context.type = RequestType::GetPayments;
        } else if (endpoint.contains("/cancel")) {
            context.type = RequestType::CancelPayment;
            context.id = endpoint.mid(9, endpoint.indexOf("/cancel") - 9);
        } else if (endpoint.startsWith("exchange-rates")) {
            context.type = RequestType::GetExchangeRates;
        }
        
        m_pendingRequests[reply] = context;
    }
}

void AsianCryptoPayment::onNetworkReply(QNetworkReply* reply) {
    if (!m_pendingRequests.contains(reply)) {
        reply->deleteLater();
        return;
    }
    
    RequestContext context = m_pendingRequests.take(reply);
    
    if (reply->error() != QNetworkReply::NoError) {
        emit error(reply->error(), reply->errorString());
        reply->deleteLater();
        return;
    }
    
    QByteArray responseData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    
    if (doc.isNull() || !doc.isObject()) {
        emit error(500, "Invalid JSON response");
        reply->deleteLater();
        return;
    }
    
    QJsonObject response = doc.object();
    
    try {
        switch (context.type) {
            case RequestType::CreatePayment: {
                Payment payment = Payment::fromJson(response);
                m_activePayments[payment.id()] = payment;
                startPaymentStatusCheck(payment);
                emit paymentCreated(payment);
                break;
            }
            case RequestType::GetPayment: {
                Payment payment = Payment::fromJson(response);
                emit paymentRetrieved(payment);
                break;
            }
            case RequestType::GetPayments: {
                QList<Payment> payments;
                int total = response["total"].toInt();
                
                if (response.contains("payments") && response["payments"].isArray()) {
                    QJsonArray paymentsArray = response["payments"].toArray();
                    
                    for (const QJsonValue& value : paymentsArray) {
                        if (value.isObject()) {
                            payments.append(Payment::fromJson(value.toObject()));
                        }
                    }
                }
                
                emit paymentsRetrieved(payments, total);
                break;
            }
            case RequestType::CancelPayment: {
                Payment payment = Payment::fromJson(response);
                stopPaymentStatusCheck(payment.id());
                emit paymentCancelled(payment);
                break;
            }
            case RequestType::GetExchangeRates: {
                QString baseCurrency = response["base_currency"].toString();
                QVariantMap rates;
                
                if (response.contains("rates") && response["rates"].isObject()) {
                    QJsonObject ratesObject = response["rates"].toObject();
                    
                    for (auto it = ratesObject.begin(); it != ratesObject.end(); ++it) {
                        rates[it.key()] = it.value().toString().toDouble();
                    }
                }
                
                emit exchangeRatesRetrieved(baseCurrency, rates);
                break;
            }
            default:
                break;
        }
    } catch (const std::exception& e) {
        emit error(500, QString::fromStdString(e.what()));
    }
    
    reply->deleteLater();
}

void AsianCryptoPayment::onQrCodeDownloaded(QNetworkReply* reply) {
    if (reply->error() != QNetworkReply::NoError) {
        emit error(reply->error(), reply->errorString());
        reply->deleteLater();
        return;
    }
    
    QByteArray imageData = reply->readAll();
    QPixmap pixmap;
    
    if (pixmap.loadFromData(imageData)) {
        emit qrCodeDownloaded(pixmap);
    } else {
        emit error(500, "Failed to load QR code image");
    }
    
    reply->deleteLater();
}

void AsianCryptoPayment::startPaymentStatusCheck(const Payment& payment) {
    if (m_paymentTimers.contains(payment.id())) {
        return;
    }
    
    QTimer* timer = new QTimer(this);
    m_paymentTimers[payment.id()] = timer;
    
    connect(timer, &QTimer::timeout, this, &AsianCryptoPayment::checkPaymentStatus);
    timer->setProperty("payment_id", payment.id());
    timer->start(10000); // Check every 10 seconds
}

void AsianCryptoPayment::stopPaymentStatusCheck(const QString& paymentId) {
    if (m_paymentTimers.contains(paymentId)) {
        QTimer* timer = m_paymentTimers.take(paymentId);
        timer->stop();
        timer->deleteLater();
    }
    
    m_activePayments.remove(paymentId);
}

void AsianCryptoPayment::checkPaymentStatus() {
    QTimer* timer = qobject_cast<QTimer*>(sender());
    if (!timer) {
        return;
    }
    
    QString paymentId = timer->property("payment_id").toString();
    if (paymentId.isEmpty()) {
        return;
    }
    
    getPayment(paymentId);
}

} // namespace AsianCryptoPay
