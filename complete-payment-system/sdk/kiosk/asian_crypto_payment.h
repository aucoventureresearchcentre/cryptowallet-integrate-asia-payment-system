/**
 * Asian Cryptocurrency Payment System - Kiosk SDK
 * Version: 1.0.0
 * 
 * This SDK provides a comprehensive interface for integrating cryptocurrency
 * payments into self-service kiosks across Asian markets, with support for 
 * regulatory compliance in Malaysia, Singapore, Indonesia, Thailand, Brunei, 
 * Cambodia, Vietnam, and Laos.
 */

#ifndef ASIAN_CRYPTO_PAYMENT_H
#define ASIAN_CRYPTO_PAYMENT_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QDateTime>
#include <QCryptographicHash>
#include <QMessageAuthenticationCode>
#include <QUuid>
#include <QTimer>
#include <QPixmap>
#include <QQmlEngine>
#include <QJSEngine>
#include <QDebug>
#include <memory>

namespace AsianCryptoPay {

/**
 * @brief Payment status enumeration
 */
enum class PaymentStatus {
    Created,
    Pending,
    Completed,
    Cancelled,
    Expired
};

/**
 * @brief Country code enumeration
 */
enum class CountryCode {
    Malaysia,    // MY
    Singapore,   // SG
    Indonesia,   // ID
    Thailand,    // TH
    Brunei,      // BN
    Cambodia,    // KH
    Vietnam,     // VN
    Laos         // LA
};

/**
 * @brief Convert CountryCode to string
 * @param code Country code
 * @return Two-letter country code string
 */
inline QString countryCodeToString(CountryCode code) {
    switch (code) {
        case CountryCode::Malaysia: return "MY";
        case CountryCode::Singapore: return "SG";
        case CountryCode::Indonesia: return "ID";
        case CountryCode::Thailand: return "TH";
        case CountryCode::Brunei: return "BN";
        case CountryCode::Cambodia: return "KH";
        case CountryCode::Vietnam: return "VN";
        case CountryCode::Laos: return "LA";
        default: return "MY";
    }
}

/**
 * @brief Convert string to CountryCode
 * @param code Two-letter country code string
 * @return CountryCode enum value
 */
inline CountryCode stringToCountryCode(const QString& code) {
    if (code == "MY") return CountryCode::Malaysia;
    if (code == "SG") return CountryCode::Singapore;
    if (code == "ID") return CountryCode::Indonesia;
    if (code == "TH") return CountryCode::Thailand;
    if (code == "BN") return CountryCode::Brunei;
    if (code == "KH") return CountryCode::Cambodia;
    if (code == "VN") return CountryCode::Vietnam;
    if (code == "LA") return CountryCode::Laos;
    return CountryCode::Malaysia;
}

/**
 * @brief Convert PaymentStatus to string
 * @param status Payment status
 * @return Status string
 */
inline QString paymentStatusToString(PaymentStatus status) {
    switch (status) {
        case PaymentStatus::Created: return "created";
        case PaymentStatus::Pending: return "pending";
        case PaymentStatus::Completed: return "completed";
        case PaymentStatus::Cancelled: return "cancelled";
        case PaymentStatus::Expired: return "expired";
        default: return "unknown";
    }
}

/**
 * @brief Convert string to PaymentStatus
 * @param status Status string
 * @return PaymentStatus enum value
 */
inline PaymentStatus stringToPaymentStatus(const QString& status) {
    if (status == "created") return PaymentStatus::Created;
    if (status == "pending") return PaymentStatus::Pending;
    if (status == "completed") return PaymentStatus::Completed;
    if (status == "cancelled") return PaymentStatus::Cancelled;
    if (status == "expired") return PaymentStatus::Expired;
    return PaymentStatus::Created;
}

/**
 * @brief Payment details class
 */
class PaymentDetails {
public:
    /**
     * @brief Constructor
     */
    PaymentDetails() {}
    
    /**
     * @brief Set payment amount
     * @param amount Payment amount
     * @return Reference to this object for method chaining
     */
    PaymentDetails& setAmount(double amount) {
        m_amount = amount;
        return *this;
    }
    
    /**
     * @brief Set fiat currency
     * @param currency Fiat currency code (e.g., MYR, SGD)
     * @return Reference to this object for method chaining
     */
    PaymentDetails& setCurrency(const QString& currency) {
        m_currency = currency;
        return *this;
    }
    
    /**
     * @brief Set cryptocurrency
     * @param cryptoCurrency Cryptocurrency code (e.g., BTC, ETH)
     * @return Reference to this object for method chaining
     */
    PaymentDetails& setCryptoCurrency(const QString& cryptoCurrency) {
        m_cryptoCurrency = cryptoCurrency;
        return *this;
    }
    
    /**
     * @brief Set payment description
     * @param description Payment description
     * @return Reference to this object for method chaining
     */
    PaymentDetails& setDescription(const QString& description) {
        m_description = description;
        return *this;
    }
    
    /**
     * @brief Set merchant order ID
     * @param orderId Merchant order ID
     * @return Reference to this object for method chaining
     */
    PaymentDetails& setOrderId(const QString& orderId) {
        m_orderId = orderId;
        return *this;
    }
    
    /**
     * @brief Set customer email
     * @param email Customer email
     * @return Reference to this object for method chaining
     */
    PaymentDetails& setCustomerEmail(const QString& email) {
        m_customerEmail = email;
        return *this;
    }
    
    /**
     * @brief Set customer name
     * @param name Customer name
     * @return Reference to this object for method chaining
     */
    PaymentDetails& setCustomerName(const QString& name) {
        m_customerName = name;
        return *this;
    }
    
    /**
     * @brief Set callback URL
     * @param url Callback URL for payment updates
     * @return Reference to this object for method chaining
     */
    PaymentDetails& setCallbackUrl(const QString& url) {
        m_callbackUrl = url;
        return *this;
    }
    
    /**
     * @brief Set success URL
     * @param url Redirect URL on successful payment
     * @return Reference to this object for method chaining
     */
    PaymentDetails& setSuccessUrl(const QString& url) {
        m_successUrl = url;
        return *this;
    }
    
    /**
     * @brief Set cancel URL
     * @param url Redirect URL on cancelled payment
     * @return Reference to this object for method chaining
     */
    PaymentDetails& setCancelUrl(const QString& url) {
        m_cancelUrl = url;
        return *this;
    }
    
    /**
     * @brief Set metadata
     * @param metadata Additional metadata
     * @return Reference to this object for method chaining
     */
    PaymentDetails& setMetadata(const QVariantMap& metadata) {
        m_metadata = metadata;
        return *this;
    }
    
    /**
     * @brief Get payment amount
     * @return Payment amount
     */
    double amount() const { return m_amount; }
    
    /**
     * @brief Get fiat currency
     * @return Fiat currency code
     */
    QString currency() const { return m_currency; }
    
    /**
     * @brief Get cryptocurrency
     * @return Cryptocurrency code
     */
    QString cryptoCurrency() const { return m_cryptoCurrency; }
    
    /**
     * @brief Get payment description
     * @return Payment description
     */
    QString description() const { return m_description; }
    
    /**
     * @brief Get merchant order ID
     * @return Merchant order ID
     */
    QString orderId() const { return m_orderId; }
    
    /**
     * @brief Get customer email
     * @return Customer email
     */
    QString customerEmail() const { return m_customerEmail; }
    
    /**
     * @brief Get customer name
     * @return Customer name
     */
    QString customerName() const { return m_customerName; }
    
    /**
     * @brief Get callback URL
     * @return Callback URL
     */
    QString callbackUrl() const { return m_callbackUrl; }
    
    /**
     * @brief Get success URL
     * @return Success URL
     */
    QString successUrl() const { return m_successUrl; }
    
    /**
     * @brief Get cancel URL
     * @return Cancel URL
     */
    QString cancelUrl() const { return m_cancelUrl; }
    
    /**
     * @brief Get metadata
     * @return Metadata
     */
    QVariantMap metadata() const { return m_metadata; }
    
    /**
     * @brief Convert to JSON object
     * @return JSON object
     */
    QJsonObject toJson() const {
        QJsonObject json;
        json["amount"] = QString::number(m_amount, 'f', 8);
        json["currency"] = m_currency;
        json["crypto_currency"] = m_cryptoCurrency;
        json["description"] = m_description;
        
        if (!m_orderId.isEmpty()) {
            json["order_id"] = m_orderId;
        }
        
        if (!m_customerEmail.isEmpty()) {
            json["customer_email"] = m_customerEmail;
        }
        
        if (!m_customerName.isEmpty()) {
            json["customer_name"] = m_customerName;
        }
        
        if (!m_callbackUrl.isEmpty()) {
            json["callback_url"] = m_callbackUrl;
        }
        
        if (!m_successUrl.isEmpty()) {
            json["success_url"] = m_successUrl;
        }
        
        if (!m_cancelUrl.isEmpty()) {
            json["cancel_url"] = m_cancelUrl;
        }
        
        if (!m_metadata.isEmpty()) {
            json["metadata"] = QJsonObject::fromVariantMap(m_metadata);
        }
        
        return json;
    }
    
private:
    double m_amount = 0.0;
    QString m_currency;
    QString m_cryptoCurrency;
    QString m_description;
    QString m_orderId;
    QString m_customerEmail;
    QString m_customerName;
    QString m_callbackUrl;
    QString m_successUrl;
    QString m_cancelUrl;
    QVariantMap m_metadata;
};

/**
 * @brief Payment class
 */
class Payment {
public:
    /**
     * @brief Constructor
     */
    Payment() {}
    
    /**
     * @brief Create from JSON object
     * @param json JSON object
     * @return Payment object
     */
    static Payment fromJson(const QJsonObject& json) {
        Payment payment;
        
        payment.m_id = json["id"].toString();
        payment.m_merchantId = json["merchant_id"].toString();
        payment.m_amount = json["amount"].toString().toDouble();
        payment.m_currency = json["currency"].toString();
        payment.m_cryptoAmount = json["crypto_amount"].toString().toDouble();
        payment.m_cryptoCurrency = json["crypto_currency"].toString();
        payment.m_description = json["description"].toString();
        payment.m_orderId = json["order_id"].toString();
        payment.m_customerEmail = json["customer_email"].toString();
        payment.m_customerName = json["customer_name"].toString();
        payment.m_address = json["address"].toString();
        payment.m_qrCodeUrl = json["qr_code_url"].toString();
        payment.m_status = stringToPaymentStatus(json["status"].toString());
        
        payment.m_createdAt = QDateTime::fromString(json["created_at"].toString(), Qt::ISODate);
        payment.m_updatedAt = QDateTime::fromString(json["updated_at"].toString(), Qt::ISODate);
        payment.m_expiresAt = QDateTime::fromString(json["expires_at"].toString(), Qt::ISODate);
        
        if (json.contains("metadata") && json["metadata"].isObject()) {
            payment.m_metadata = json["metadata"].toObject().toVariantMap();
        }
        
        return payment;
    }
    
    /**
     * @brief Get payment ID
     * @return Payment ID
     */
    QString id() const { return m_id; }
    
    /**
     * @brief Get merchant ID
     * @return Merchant ID
     */
    QString merchantId() const { return m_merchantId; }
    
    /**
     * @brief Get payment amount
     * @return Payment amount
     */
    double amount() const { return m_amount; }
    
    /**
     * @brief Get fiat currency
     * @return Fiat currency code
     */
    QString currency() const { return m_currency; }
    
    /**
     * @brief Get cryptocurrency amount
     * @return Cryptocurrency amount
     */
    double cryptoAmount() const { return m_cryptoAmount; }
    
    /**
     * @brief Get cryptocurrency
     * @return Cryptocurrency code
     */
    QString cryptoCurrency() const { return m_cryptoCurrency; }
    
    /**
     * @brief Get payment description
     * @return Payment description
     */
    QString description() const { return m_description; }
    
    /**
     * @brief Get merchant order ID
     * @return Merchant order ID
     */
    QString orderId() const { return m_orderId; }
    
    /**
     * @brief Get customer email
     * @return Customer email
     */
    QString customerEmail() const { return m_customerEmail; }
    
    /**
     * @brief Get customer name
     * @return Customer name
     */
    QString customerName() const { return m_customerName; }
    
    /**
     * @brief Get cryptocurrency address
     * @return Cryptocurrency address
     */
    QString address() const { return m_address; }
    
    /**
     * @brief Get QR code URL
     * @return QR code URL
     */
    QString qrCodeUrl() const { return m_qrCodeUrl; }
    
    /**
     * @brief Get payment status
     * @return Payment status
     */
    PaymentStatus status() const { return m_status; }
    
    /**
     * @brief Get payment status as string
     * @return Payment status string
     */
    QString statusString() const { return paymentStatusToString(m_status); }
    
    /**
     * @brief Get creation time
     * @return Creation time
     */
    QDateTime createdAt() const { return m_createdAt; }
    
    /**
     * @brief Get last update time
     * @return Last update time
     */
    QDateTime updatedAt() const { return m_updatedAt; }
    
    /**
     * @brief Get expiration time
     * @return Expiration time
     */
    QDateTime expiresAt() const { return m_expiresAt; }
    
    /**
     * @brief Get metadata
     * @return Metadata
     */
    QVariantMap metadata() const { return m_metadata; }
    
    /**
     * @brief Check if payment is completed
     * @return Whether payment is completed
     */
    bool isCompleted() const { return m_status == PaymentStatus::Completed; }
    
    /**
     * @brief Check if payment is pending
     * @return Whether payment is pending
     */
    bool isPending() const { return m_status == PaymentStatus::Pending; }
    
    /**
     * @brief Check if payment is expired
     * @return Whether payment is expired
     */
    bool isExpired() const { return m_status == PaymentStatus::Expired; }
    
    /**
     * @brief Check if payment is cancelled
     * @return Whether payment is cancelled
     */
    bool isCancelled() const { return m_status == PaymentStatus::Cancelled; }
    
    /**
     * @brief Convert to JSON object
     * @return JSON object
     */
    QJsonObject toJson() const {
        QJsonObject json;
        json["id"] = m_id;
        json["merchant_id"] = m_merchantId;
        json["amount"] = QString::number(m_amount, 'f', 8);
        json["currency"] = m_currency;
        json["crypto_amount"] = QString::number(m_cryptoAmount, 'f', 8);
        json["crypto_currency"] = m_cryptoCurrency;
        json["description"] = m_description;
        json["order_id"] = m_orderId;
        json["customer_email"] = m_customerEmail;
        json["customer_name"] = m_customerName;
        json["address"] = m_address;
        json["qr_code_url"] = m_qrCodeUrl;
        json["status"] = paymentStatusToString(m_status);
        json["created_at"] = m_createdAt.toString(Qt::ISODate);
        json["updated_at"] = m_updatedAt.toString(Qt::ISODate);
        json["expires_at"] = m_expiresAt.toString(Qt::ISODate);
        
        if (!m_metadata.isEmpty()) {
            json["metadata"] = QJsonObject::fromVariantMap(m_metadata);
        }
        
        return json;
    }
    
private:
    QString m_id;
    QString m_merchantId;
    double m_amount = 0.0;
    QString m_currency;
    double m_cryptoAmount = 0.0;
    QString m_cryptoCurrency;
    QString m_description;
    QString m_orderId;
    QString m_customerEmail;
    QString m_customerName;
    QString m_address;
    QString m_qrCodeUrl;
    PaymentStatus m_status = PaymentStatus::Created;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
    QDateTime m_expiresAt;
    QVariantMap m_metadata;
};

/**
 * @brief Payment filters class
 */
class PaymentFilters {
public:
    /**
     * @brief Constructor
     */
    PaymentFilters() {}
    
    /**
     * @brief Set status filter
     * @param status Payment status
     * @return Reference to this object for method chaining
     */
    PaymentFilters& setStatus(PaymentStatus status) {
        m_status = status;
        return *this;
    }
    
    /**
     * @brief Set date range start
     * @param fromDate Date range start
     * @return Reference to this object for method chaining
     */
    PaymentFilters& setFromDate(const QDateTime& fromDate) {
        m_fromDate = fromDate;
        return *this;
    }
    
    /**
     * @brief Set date range end
     * @param toDate Date range end
     * @return Reference to this object for method chaining
     */
    PaymentFilters& setToDate(const QDateTime& toDate) {
        m_toDate = toDate;
        return *this;
    }
    
    /**
     * @brief Set result limit
     * @param limit Number of results to return
     * @return Reference to this object for method chaining
     */
    PaymentFilters& setLimit(int limit) {
        m_limit = limit;
        return *this;
    }
    
    /**
     * @brief Set result offset
     * @param offset Offset for pagination
     * @return Reference to this object for method chaining
     */
    PaymentFilters& setOffset(int offset) {
        m_offset = offset;
        return *this;
    }
    
    /**
     * @brief Get status filter
     * @return Payment status
     */
    PaymentStatus status() const { return m_status; }
    
    /**
     * @brief Get date range start
     * @return Date range start
     */
    QDateTime fromDate() const { return m_fromDate; }
    
    /**
     * @brief Get date range end
     * @return Date range end
     */
    QDateTime toDate() const { return m_toDate; }
    
    /**
     * @brief Get result limit
     * @return Number of results to return
     */
    int limit() const { return m_limit; }
    
    /**
     * @brief Get result offset
     * @return Offset for pagination
     */
    int offset() const { return m_offset; }
    
    /**
     * @brief Build query string
     * @return Query string
     */
    QString buildQueryString() const {
        QStringList params;
        
        if (m_status != PaymentStatus::Created) {
            params << QString("status=%1").arg(paymentStatusToString(m_status));
        }
        
        if (m_fromDate.isValid()) {
            params << QString("from_date=%1").arg(m_fromDate.toString("yyyy-MM-dd"));
        }
        
        if (m_toDate.isValid()) {
            params << QString("to_date=%1").arg(m_toDate.toString("yyyy-MM-dd"));
        }
        
        if (m_limit > 0) {
            params << QString("limit=%1").arg(m_limit);
        }
        
        if (m_offset > 0) {
            params << QString("offset=%1").arg(m_offset);
        }
        
        return params.join("&");
    }
    
private:
    PaymentStatus m_status = PaymentStatus::Created;
    QDateTime m_fromDate;
    QDateTime m_toDate;
    int m_limit = 20;
    int m_offset = 0;
};

// Forward declarations
class CountryComplianceModule;
class SecurityModule;

/**
 * @brief Main SDK class for Asian Cryptocurrency Payment System
 */
class AsianCryptoPayment : public QObject {
    Q_OBJECT
    
public:
    /**
     * @brief Constructor
     * @param apiKey Merchant API key
     * @param merchantId Merchant ID
     * @param countryCode Country code
     * @param parent Parent QObject
     */
    AsianCryptoPayment(const QString& apiKey, const QString& merchantId, CountryCode countryCode, QObject* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~AsianCryptoPayment();
    
    /**
     * @brief Set test mode
     * @param testMode Whether to use test mode
     */
    void setTestMode(bool testMode);
    
    /**
     * @brief Set custom API endpoint
     * @param apiEndpoint Custom API endpoint
     */
    void setApiEndpoint(const QString& apiEndpoint);
    
    /**
     * @brief Set supported cryptocurrencies
     * @param supportedCryptocurrencies List of supported cryptocurrencies
     */
    void setSupportedCryptocurrencies(const QStringList& supportedCryptocurrencies);
    
    /**
     * @brief Set webhook configuration
     * @param webhookEndpoint Webhook endpoint URL
     * @param webhookSecret Webhook secret for signature verification
     */
    void setWebhookConfig(const QString& webhookEndpoint, const QString& webhookSecret);
    
    /**
     * @brief Get API key
     * @return API key
     */
    QString apiKey() const { return m_apiKey; }
    
    /**
     * @brief Get merchant ID
     * @return Merchant ID
     */
    QString merchantId() const { return m_merchantId; }
    
    /**
     * @brief Get country code
     * @return Country code
     */
    CountryCode countryCode() const { return m_countryCode; }
    
    /**
     * @brief Get country code as string
     * @return Two-letter country code string
     */
    QString countryCodeString() const { return countryCodeToString(m_countryCode); }
    
    /**
     * @brief Get test mode
     * @return Whether test mode is enabled
     */
    bool testMode() const { return m_testMode; }
    
    /**
     * @brief Get API endpoint
     * @return API endpoint
     */
    QString apiEndpoint() const { return m_apiEndpoint; }
    
    /**
     * @brief Get supported cryptocurrencies
     * @return List of supported cryptocurrencies
     */
    QStringList supportedCryptocurrencies() const { return m_supportedCryptocurrencies; }
    
    /**
     * @brief Create a new cryptocurrency payment
     * @param paymentDetails Payment details
     */
    void createPayment(const PaymentDetails& paymentDetails);
    
    /**
     * @brief Get payment details by ID
     * @param paymentId Payment ID
     */
    void getPayment(const QString& paymentId);
    
    /**
     * @brief Get list of payments
     * @param filters Filter parameters
     */
    void getPayments(const PaymentFilters& filters = PaymentFilters());
    
    /**
     * @brief Cancel a payment
     * @param paymentId Payment ID
     */
    void cancelPayment(const QString& paymentId);
    
    /**
     * @brief Get current exchange rates
     * @param baseCurrency Base currency
     * @param cryptoCurrencies List of cryptocurrencies to get rates for
     */
    void getExchangeRates(const QString& baseCurrency, const QStringList& cryptoCurrencies = QStringList());
    
    /**
     * @brief Verify webhook signature
     * @param signature Webhook signature
     * @param body Raw webhook body
     * @return Whether signature is valid
     */
    bool verifyWebhookSignature(const QString& signature, const QString& body);
    
    /**
     * @brief Process webhook event
     * @param event Webhook event
     * @param signature Webhook signature
     * @return Whether event was processed successfully
     */
    bool processWebhookEvent(const QJsonObject& event, const QString& signature);
    
    /**
     * @brief Download QR code image
     * @param url QR code URL
     */
    void downloadQrCode(const QString& url);
    
signals:
    /**
     * @brief Emitted when payment is created
     * @param payment Payment object
     */
    void paymentCreated(const Payment& payment);
    
    /**
     * @brief Emitted when payment is retrieved
     * @param payment Payment object
     */
    void paymentRetrieved(const Payment& payment);
    
    /**
     * @brief Emitted when payments list is retrieved
     * @param payments List of payments
     * @param total Total number of payments
     */
    void paymentsRetrieved(const QList<Payment>& payments, int total);
    
    /**
     * @brief Emitted when payment is cancelled
     * @param payment Payment object
     */
    void paymentCancelled(const Payment& payment);
    
    /**
     * @brief Emitted when exchange rates are retrieved
     * @param baseCurrency Base currency
     * @param rates Map of cryptocurrency to exchange rate
     */
    void exchangeRatesRetrieved(const QString& baseCurrency, const QVariantMap& rates);
    
    /**
     * @brief Emitted when QR code is downloaded
     * @param pixmap QR code image
     */
    void qrCodeDownloaded(const QPixmap& pixmap);
    
    /**
     * @brief Emitted when payment status is updated
     * @param payment Payment object
     */
    void paymentStatusUpdated(const Payment& payment);
    
    /**
     * @brief Emitted when error occurs
     * @param errorCode Error code
     * @param errorMessage Error message
     */
    void error(int errorCode, const QString& errorMessage);
    
private slots:
    void onNetworkReply(QNetworkReply* reply);
    void onQrCodeDownloaded(QNetworkReply* reply);
    void checkPaymentStatus();
    
private:
    // Configuration
    QString m_apiKey;
    QString m_merchantId;
    CountryCode m_countryCode;
    bool m_testMode = false;
    QString m_apiEndpoint = "https://api.asiancryptopay.com";
    QStringList m_supportedCryptocurrencies;
    QVariantMap m_webhookConfig;
    
    // Network
    QNetworkAccessManager* m_networkManager;
    
    // Modules
    std::unique_ptr<CountryComplianceModule> m_countryModule;
    std::unique_ptr<SecurityModule> m_securityModule;
    
    // Active payments
    QMap<QString, Payment> m_activePayments;
    QMap<QString, QTimer*> m_paymentTimers;
    
    // Request tracking
    enum class RequestType {
        CreatePayment,
        GetPayment,
        GetPayments,
        CancelPayment,
        GetExchangeRates,
        DownloadQrCode
    };
    
    struct RequestContext {
        RequestType type;
        QString id;
        QVariantMap data;
    };
    
    QMap<QNetworkReply*, RequestContext> m_pendingRequests;
    
    // Methods
    void validatePaymentDetails(const PaymentDetails& paymentDetails);
    QNetworkRequest createApiRequest(const QString& endpoint, const QJsonObject& data = QJsonObject());
    void makeApiRequest(const QString& endpoint, const QString& method, const QJsonObject& data = QJsonObject());
    void startPaymentStatusCheck(const Payment& payment);
    void stopPaymentStatusCheck(const QString& paymentId);
};

/**
 * @brief Security module for cryptographic operations
 */
class SecurityModule {
public:
    /**
     * @brief Constructor
     * @param apiKey API key for signature generation
     */
    SecurityModule(const QString& apiKey) : m_apiKey(apiKey) {}
    
    /**
     * @brief Generate signature for API request
     * @param payload Request payload
     * @param timestamp Request timestamp
     * @return Signature
     */
    QString generateSignature(const QString& payload, const QString& timestamp) {
        QString message = timestamp + "." + payload;
        return hmacSha256(m_apiKey, message);
    }
    
    /**
     * @brief Verify webhook signature
     * @param signature Webhook signature
     * @param payload Webhook payload
     * @param secret Webhook secret
     * @return Whether signature is valid
     */
    bool verifySignature(const QString& signature, const QString& payload, const QString& secret) {
        QString expectedSignature = hmacSha256(secret, payload);
        return expectedSignature == signature;
    }
    
private:
    QString m_apiKey;
    
    /**
     * @brief Generate HMAC-SHA256 signature
     * @param key Secret key
     * @param message Message to sign
     * @return HMAC-SHA256 signature
     */
    QString hmacSha256(const QString& key, const QString& message) {
        return QMessageAuthenticationCode::hash(
            message.toUtf8(),
            key.toUtf8(),
            QCryptographicHash::Sha256
        ).toHex();
    }
};

/**
 * @brief Interface for country-specific compliance modules
 */
class CountryComplianceModule {
public:
    virtual ~CountryComplianceModule() {}
    
    /**
     * @brief Validate payment according to country-specific regulations
     * @param paymentDetails Payment details
     * @throws std::invalid_argument if validation fails
     */
    virtual void validatePayment(const PaymentDetails& paymentDetails) = 0;
    
    /**
     * @brief Get country name
     * @return Country name
     */
    virtual QString countryName() const = 0;
    
    /**
     * @brief Get currency code
     * @return Currency code
     */
    virtual QString currencyCode() const = 0;
    
    /**
     * @brief Get regulator name
     * @return Regulator name
     */
    virtual QString regulator() const = 0;
    
    /**
     * @brief Get KYC threshold
     * @return KYC threshold
     */
    virtual double kycThreshold() const = 0;
};

/**
 * @brief Malaysia compliance module
 */
class MalaysiaComplianceModule : public CountryComplianceModule {
public:
    void validatePayment(const PaymentDetails& paymentDetails) override {
        if (currencyCode() == paymentDetails.currency()) {
            if (paymentDetails.amount() >= kycThreshold() && 
                    paymentDetails.customerName().isEmpty()) {
                throw std::invalid_argument("KYC information required for payments above " + 
                        QString::number(kycThreshold()).toStdString() + " " + currencyCode().toStdString());
            }
        }
    }
    
    QString countryName() const override { return "Malaysia"; }
    QString currencyCode() const override { return "MYR"; }
    QString regulator() const override { return "Securities Commission Malaysia (SC)"; }
    double kycThreshold() const override { return 3000.0; }
};

/**
 * @brief Singapore compliance module
 */
class SingaporeComplianceModule : public CountryComplianceModule {
public:
    void validatePayment(const PaymentDetails& paymentDetails) override {
        if (currencyCode() == paymentDetails.currency()) {
            if (paymentDetails.amount() >= kycThreshold() && 
                    (paymentDetails.customerName().isEmpty() || paymentDetails.customerEmail().isEmpty())) {
                throw std::invalid_argument("KYC information (name and email) required for payments above " + 
                        QString::number(kycThreshold()).toStdString() + " " + currencyCode().toStdString());
            }
            
            // Travel Rule compliance
            if (paymentDetails.amount() >= 1000.0) {
                qDebug() << "Travel Rule applies to this transaction";
                // Additional Travel Rule implementation would go here
            }
        }
    }
    
    QString countryName() const override { return "Singapore"; }
    QString currencyCode() const override { return "SGD"; }
    QString regulator() const override { return "Monetary Authority of Singapore (MAS)"; }
    double kycThreshold() const override { return 1000.0; }
};

/**
 * @brief Indonesia compliance module
 */
class IndonesiaComplianceModule : public CountryComplianceModule {
public:
    void validatePayment(const PaymentDetails& paymentDetails) override {
        if (currencyCode() == paymentDetails.currency()) {
            if (paymentDetails.amount() >= kycThreshold() && 
                    paymentDetails.customerName().isEmpty()) {
                throw std::invalid_argument("KYC information required for payments above " + 
                        QString::number(kycThreshold()).toStdString() + " " + currencyCode().toStdString());
            }
        }
    }
    
    QString countryName() const override { return "Indonesia"; }
    QString currencyCode() const override { return "IDR"; }
    QString regulator() const override { return "Commodity Futures Trading Regulatory Agency (Bappebti)"; }
    double kycThreshold() const override { return 2000000.0; }
};

/**
 * @brief Thailand compliance module
 */
class ThailandComplianceModule : public CountryComplianceModule {
public:
    void validatePayment(const PaymentDetails& paymentDetails) override {
        if (currencyCode() == paymentDetails.currency()) {
            if (paymentDetails.amount() >= kycThreshold() && 
                    paymentDetails.customerName().isEmpty()) {
                throw std::invalid_argument("KYC information required for payments above " + 
                        QString::number(kycThreshold()).toStdString() + " " + currencyCode().toStdString());
            }
        }
    }
    
    QString countryName() const override { return "Thailand"; }
    QString currencyCode() const override { return "THB"; }
    QString regulator() const override { return "Securities and Exchange Commission (SEC)"; }
    double kycThreshold() const override { return 50000.0; }
};

/**
 * @brief Brunei compliance module
 */
class BruneiComplianceModule : public CountryComplianceModule {
public:
    void validatePayment(const PaymentDetails& paymentDetails) override {
        if (currencyCode() == paymentDetails.currency()) {
            if (paymentDetails.amount() >= kycThreshold() && 
                    paymentDetails.customerName().isEmpty()) {
                throw std::invalid_argument("KYC information required for payments above " + 
                        QString::number(kycThreshold()).toStdString() + " " + currencyCode().toStdString());
            }
        }
    }
    
    QString countryName() const override { return "Brunei"; }
    QString currencyCode() const override { return "BND"; }
    QString regulator() const override { return "Autoriti Monetari Brunei Darussalam (AMBD)"; }
    double kycThreshold() const override { return 5000.0; }
};

/**
 * @brief Cambodia compliance module
 */
class CambodiaComplianceModule : public CountryComplianceModule {
public:
    void validatePayment(const PaymentDetails& paymentDetails) override {
        if (currencyCode() == paymentDetails.currency()) {
            if (paymentDetails.amount() >= kycThreshold() && 
                    paymentDetails.customerName().isEmpty()) {
                throw std::invalid_argument("KYC information required for payments above " + 
                        QString::number(kycThreshold()).toStdString() + " " + currencyCode().toStdString());
            }
        }
    }
    
    QString countryName() const override { return "Cambodia"; }
    QString currencyCode() const override { return "KHR"; }
    QString regulator() const override { return "National Bank of Cambodia (NBC)"; }
    double kycThreshold() const override { return 4000000.0; }
};

/**
 * @brief Vietnam compliance module
 */
class VietnamComplianceModule : public CountryComplianceModule {
public:
    void validatePayment(const PaymentDetails& paymentDetails) override {
        if (currencyCode() == paymentDetails.currency()) {
            if (paymentDetails.amount() >= kycThreshold() && 
                    paymentDetails.customerName().isEmpty()) {
                throw std::invalid_argument("KYC information required for payments above " + 
                        QString::number(kycThreshold()).toStdString() + " " + currencyCode().toStdString());
            }
        }
    }
    
    QString countryName() const override { return "Vietnam"; }
    QString currencyCode() const override { return "VND"; }
    QString regulator() const override { return "State Bank of Vietnam (SBV)"; }
    double kycThreshold() const override { return 20000000.0; }
};

/**
 * @brief Laos compliance module
 */
class LaosComplianceModule : public CountryComplianceModule {
public:
    void validatePayment(const PaymentDetails& paymentDetails) override {
        if (currencyCode() == paymentDetails.currency()) {
            if (paymentDetails.amount() >= kycThreshold() && 
                    paymentDetails.customerName().isEmpty()) {
                throw std::invalid_argument("KYC information required for payments above " + 
                        QString::number(kycThreshold()).toStdString() + " " + currencyCode().toStdString());
            }
        }
    }
    
    QString countryName() const override { return "Laos"; }
    QString currencyCode() const override { return "LAK"; }
    QString regulator() const override { return "Bank of the Lao PDR (BOL)"; }
    double kycThreshold() const override { return 9000000.0; }
};

/**
 * @brief Create country-specific compliance module
 * @param countryCode Country code
 * @return CountryComplianceModule instance
 */
inline std::unique_ptr<CountryComplianceModule> createCountryModule(CountryCode countryCode) {
    switch (countryCode) {
        case CountryCode::Malaysia:
            return std::make_unique<MalaysiaComplianceModule>();
        case CountryCode::Singapore:
            return std::make_unique<SingaporeComplianceModule>();
        case CountryCode::Indonesia:
            return std::make_unique<IndonesiaComplianceModule>();
        case CountryCode::Thailand:
            return std::make_unique<ThailandComplianceModule>();
        case CountryCode::Brunei:
            return std::make_unique<BruneiComplianceModule>();
        case CountryCode::Cambodia:
            return std::make_unique<CambodiaComplianceModule>();
        case CountryCode::Vietnam:
            return std::make_unique<VietnamComplianceModule>();
        case CountryCode::Laos:
            return std::make_unique<LaosComplianceModule>();
        default:
            return std::make_unique<MalaysiaComplianceModule>();
    }
}

/**
 * @brief AsianCryptoPayment implementation
 */
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

// Register types for QML
static QObject* createAsianCryptoPaymentSingleton(QQmlEngine* engine, QJSEngine* scriptEngine) {
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    
    // This is just a placeholder for QML registration
    // Actual instances should be created in C++ code
    return new AsianCryptoPay::AsianCryptoPayment("", "", AsianCryptoPay::CountryCode::Malaysia);
}

static void registerAsianCryptoPaymentTypes() {
    qmlRegisterSingletonType<AsianCryptoPay::AsianCryptoPayment>(
        "AsianCryptoPay", 1, 0, "AsianCryptoPayment", createAsianCryptoPaymentSingleton);
    
    qRegisterMetaType<AsianCryptoPay::Payment>("Payment");
    qRegisterMetaType<QList<AsianCryptoPay::Payment>>("QList<Payment>");
    qRegisterMetaType<AsianCryptoPay::PaymentStatus>("PaymentStatus");
    qRegisterMetaType<AsianCryptoPay::CountryCode>("CountryCode");
}

Q_COREAPP_STARTUP_FUNCTION(registerAsianCryptoPaymentTypes)

#endif // ASIAN_CRYPTO_PAYMENT_H
