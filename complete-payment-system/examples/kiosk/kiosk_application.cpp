#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QMessageBox>
#include <QTimer>
#include <QPixmap>
#include <QFont>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QDateTime>

#include "../../../sdk/kiosk/asian_crypto_payment.cpp"

/**
 * Example Kiosk Application integrating the Asian Cryptocurrency Payment System
 */
class KioskApplication : public QMainWindow {
    Q_OBJECT

public:
    KioskApplication(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Self-Service Kiosk - Asian Crypto Payment Example");
        setMinimumSize(800, 600);
        
        // Initialize payment SDK
        try {
            paymentSDK = new AsianCryptoPayment(
                "demo_api_key",
                "demo_merchant_id",
                "SG", // Default to Singapore
                "https://api.asiancryptopay.com/v1",
                true
            );
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", QString("Failed to initialize payment SDK: %1").arg(e.what()));
        }
        
        setupUI();
    }
    
    ~KioskApplication() {
        delete paymentSDK;
    }

private slots:
    void onCountryChanged(int index) {
        // Get selected country code
        selectedCountry = countryCodesList[index];
        
        // Update currency
        selectedCurrency = getDefaultCurrencyForCountry(selectedCountry);
        currencyLabel->setText(QString("Amount (%1):").arg(selectedCurrency.c_str()));
        
        // Reinitialize payment SDK
        try {
            delete paymentSDK;
            paymentSDK = new AsianCryptoPayment(
                "demo_api_key",
                "demo_merchant_id",
                selectedCountry,
                "https://api.asiancryptopay.com/v1",
                true
            );
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", QString("Failed to reinitialize payment SDK: %1").arg(e.what()));
        }
    }
    
    void onCryptoCurrencyChanged(int index) {
        // Get selected cryptocurrency
        selectedCryptoCurrency = cryptoCurrenciesList[index];
    }
    
    void onAddToCartClicked() {
        // Get selected product
        int productIndex = productComboBox->currentIndex();
        double productPrice = productPrices[productIndex];
        QString productName = productComboBox->currentText();
        
        // Get quantity
        int quantity = quantitySpinBox->value();
        
        // Calculate total
        double itemTotal = productPrice * quantity;
        cartTotal += itemTotal;
        
        // Add to cart display
        QString cartItem = QString("%1 x %2 - $%3")
            .arg(quantity)
            .arg(productName)
            .arg(itemTotal, 0, 'f', 2);
        
        cartListWidget->addItem(cartItem);
        
        // Update total display
        totalLabel->setText(QString("Total: $%1").arg(cartTotal, 0, 'f', 2));
        
        // Enable checkout button if cart has items
        checkoutButton->setEnabled(true);
    }
    
    void onCheckoutClicked() {
        // Switch to payment view
        stackedWidget->setCurrentIndex(1);
        
        // Update payment amount
        paymentAmountLabel->setText(QString("Amount: $%1 (%2)")
            .arg(cartTotal, 0, 'f', 2)
            .arg(selectedCurrency.c_str()));
    }
    
    void onPayWithCryptoClicked() {
        try {
            // Create payment
            Json::Value payment = paymentSDK->createPayment(
                cartTotal,
                selectedCurrency,
                selectedCryptoCurrency
            );
            
            // Extract payment details
            std::string paymentAddress = payment["payment_address"].asString();
            std::string transactionId = payment["transaction_id"].asString();
            double cryptoAmount = payment["crypto_amount"].asDouble();
            std::string cryptoCurrencyCode = payment["crypto_currency"].asString();
            std::string expiresAt = payment["expires_at"].asString();
            
            // Update UI
            cryptoAmountLabel->setText(QString("Amount: %1 %2")
                .arg(cryptoAmount)
                .arg(cryptoCurrencyCode.c_str()));
            
            addressLabel->setText(QString("Address: %1").arg(paymentAddress.c_str()));
            expiryLabel->setText(QString("Expires: %1").arg(expiresAt.c_str()));
            statusLabel->setText("Status: Waiting for payment...");
            
            // In a real implementation, we would display a QR code
            // For this example, we'll just show a placeholder
            qrCodeLabel->setText("QR Code for: " + QString::fromStdString(paymentAddress));
            qrCodeLabel->setAlignment(Qt::AlignCenter);
            qrCodeLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
            qrCodeLabel->setMinimumSize(200, 200);
            
            // Show payment details
            paymentDetailsWidget->setVisible(true);
            
            // Start checking payment status
            startCheckingPaymentStatus(transactionId);
            
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", QString("Payment creation failed: %1").arg(e.what()));
            statusLabel->setText(QString("Error: %1").arg(e.what()));
        }
    }
    
    void onCancelPaymentClicked() {
        // Return to shopping view
        stackedWidget->setCurrentIndex(0);
        
        // Hide payment details
        paymentDetailsWidget->setVisible(false);
        
        // Reset status
        statusLabel->setText("");
    }
    
    void onBackToShoppingClicked() {
        // Clear cart
        cartListWidget->clear();
        cartTotal = 0.0;
        totalLabel->setText("Total: $0.00");
        checkoutButton->setEnabled(false);
        
        // Return to shopping view
        stackedWidget->setCurrentIndex(0);
        
        // Hide payment details
        paymentDetailsWidget->setVisible(false);
        
        // Reset status
        statusLabel->setText("");
    }

private:
    AsianCryptoPayment* paymentSDK;
    std::string selectedCountry = "SG";
    std::string selectedCurrency = "SGD";
    std::string selectedCryptoCurrency = "BTC";
    double cartTotal = 0.0;
    
    // UI Elements
    QWidget* centralWidget;
    QStackedWidget* stackedWidget;
    
    // Shopping View
    QWidget* shoppingWidget;
    QComboBox* countryComboBox;
    QComboBox* productComboBox;
    QSpinBox* quantitySpinBox;
    QPushButton* addToCartButton;
    QListWidget* cartListWidget;
    QLabel* totalLabel;
    QPushButton* checkoutButton;
    
    // Payment View
    QWidget* paymentWidget;
    QLabel* paymentAmountLabel;
    QComboBox* cryptoCurrencyComboBox;
    QPushButton* payWithCryptoButton;
    QPushButton* cancelPaymentButton;
    QWidget* paymentDetailsWidget;
    QLabel* qrCodeLabel;
    QLabel* cryptoAmountLabel;
    QLabel* addressLabel;
    QLabel* expiryLabel;
    QLabel* statusLabel;
    QPushButton* backToShoppingButton;
    QLabel* currencyLabel;
    
    // Data
    std::vector<std::string> countryCodesList = {"MY", "SG", "ID", "TH", "BN", "KH", "VN", "LA"};
    std::vector<std::string> cryptoCurrenciesList = {"BTC", "ETH", "USDT"};
    std::vector<double> productPrices = {10.99, 24.99, 49.99, 99.99, 199.99};
    
    void setupUI() {
        centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
        
        // Create stacked widget for different views
        stackedWidget = new QStackedWidget();
        
        // Create shopping view
        setupShoppingView();
        
        // Create payment view
        setupPaymentView();
        
        // Add views to stacked widget
        stackedWidget->addWidget(shoppingWidget);
        stackedWidget->addWidget(paymentWidget);
        
        // Start with shopping view
        stackedWidget->setCurrentIndex(0);
        
        mainLayout->addWidget(stackedWidget);
    }
    
    void setupShoppingView() {
        shoppingWidget = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(shoppingWidget);
        
        // Header
        QLabel* headerLabel = new QLabel("Self-Service Kiosk");
        QFont headerFont = headerLabel->font();
        headerFont.setPointSize(24);
        headerFont.setBold(true);
        headerLabel->setFont(headerFont);
        headerLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(headerLabel);
        
        // Country selection
        QGroupBox* countryGroupBox = new QGroupBox("Select Your Country");
        QHBoxLayout* countryLayout = new QHBoxLayout();
        
        countryComboBox = new QComboBox();
        countryComboBox->addItem("🇲🇾 Malaysia");
        countryComboBox->addItem("🇸🇬 Singapore");
        countryComboBox->addItem("🇮🇩 Indonesia");
        countryComboBox->addItem("🇹🇭 Thailand");
        countryComboBox->addItem("🇧🇳 Brunei");
        countryComboBox->addItem("🇰🇭 Cambodia");
        countryComboBox->addItem("🇻🇳 Vietnam");
        countryComboBox->addItem("🇱🇦 Laos");
        
        // Default to Singapore (index 1)
        countryComboBox->setCurrentIndex(1);
        
        countryLayout->addWidget(countryComboBox);
        countryGroupBox->setLayout(countryLayout);
        layout->addWidget(countryGroupBox);
        
        // Product selection
        QGroupBox* productGroupBox = new QGroupBox("Select Product");
        QVBoxLayout* productLayout = new QVBoxLayout();
        
        productComboBox = new QComboBox();
        productComboBox->addItem("Basic Meal - $10.99");
        productComboBox->addItem("Premium Meal - $24.99");
        productComboBox->addItem("Family Meal - $49.99");
        productComboBox->addItem("Party Pack - $99.99");
        productComboBox->addItem("Catering Package - $199.99");
        
        QHBoxLayout* quantityLayout = new QHBoxLayout();
        QLabel* quantityLabel = new QLabel("Quantity:");
        quantitySpinBox = new QSpinBox();
        quantitySpinBox->setMinimum(1);
        quantitySpinBox->setMaximum(10);
        quantityLayout->addWidget(quantityLabel);
        quantityLayout->addWidget(quantitySpinBox);
        
        addToCartButton = new QPushButton("Add to Cart");
        
        productLayout->addWidget(productComboBox);
        productLayout->addLayout(quantityLayout);
        productLayout->addWidget(addToCartButton);
        
        productGroupBox->setLayout(productLayout);
        layout->addWidget(productGroupBox);
        
        // Cart
        QGroupBox* cartGroupBox = new QGroupBox("Your Cart");
        QVBoxLayout* cartLayout = new QVBoxLayout();
        
        cartListWidget = new QListWidget();
        totalLabel = new QLabel("Total: $0.00");
        QFont totalFont = totalLabel->font();
        totalFont.setBold(true);
        totalLabel->setFont(totalFont);
        
        checkoutButton = new QPushButton("Checkout");
        checkoutButton->setEnabled(false);
        
        cartLayout->addWidget(cartListWidget);
        cartLayout->addWidget(totalLabel);
        cartLayout->addWidget(checkoutButton);
        
        cartGroupBox->setLayout(cartLayout);
        layout->addWidget(cartGroupBox);
        
        // Connect signals
        connect(countryComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCountryChanged(int)));
        connect(addToCartButton, SIGNAL(clicked()), this, SLOT(onAddToCartClicked()));
        connect(checkoutButton, SIGNAL(clicked()), this, SLOT(onCheckoutClicked()));
    }
    
    void setupPaymentView() {
        paymentWidget = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(paymentWidget);
        
        // Header
        QLabel* headerLabel = new QLabel("Payment");
        QFont headerFont = headerLabel->font();
        headerFont.setPointSize(24);
        headerFont.setBold(true);
        headerLabel->setFont(headerFont);
        headerLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(headerLabel);
        
        // Payment amount
        paymentAmountLabel = new QLabel("Amount: $0.00");
        QFont amountFont = paymentAmountLabel->font();
        amountFont.setPointSize(18);
        paymentAmountLabel->setFont(amountFont);
        paymentAmountLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(paymentAmountLabel);
        
        // Payment method
        QGroupBox* paymentMethodGroupBox = new QGroupBox("Select Payment Method");
        QVBoxLayout* paymentMethodLayout = new QVBoxLayout();
        
        QRadioButton* cryptoRadioButton = new QRadioButton("Cryptocurrency");
        cryptoRadioButton->setChecked(true);
        
        QHBoxLayout* cryptoLayout = new QHBoxLayout();
        QLabel* cryptoLabel = new QLabel("Select Cryptocurrency:");
        cryptoCurrencyComboBox = new QComboBox();
        cryptoCurrencyComboBox->addItem("Bitcoin (BTC)");
        cryptoCurrencyComboBox->addItem("Ethereum (ETH)");
        cryptoCurrencyComboBox->addItem("Tether (USDT)");
        cryptoLayout->addWidget(cryptoLabel);
        cryptoLayout->addWidget(cryptoCurrencyComboBox);
        
        paymentMethodLayout->addWidget(cryptoRadioButton);
        paymentMethodLayout->addLayout(cryptoLayout);
        
        paymentMethodGroupBox->setLayout(paymentMethodLayout);
        layout->addWidget(paymentMethodGroupBox);
        
        // Payment buttons
        QHBoxLayout* paymentButtonsLayout = new QHBoxLayout();
        payWithCryptoButton = new QPushButton("Pay with Cryptocurrency");
        cancelPaymentButton = new QPushButton("Cancel");
        
        paymentButtonsLayout->addWidget(cancelPaymentButton);
        paymentButtonsLayout->addWidget(payWithCryptoButton);
        
        layout->addLayout(paymentButtonsLayout);
        
        // Payment details (initially hidden)
        paymentDetailsWidget = new QWidget();
        QVBoxLayout* paymentDetailsLayout = new QVBoxLayout(paymentDetailsWidget);
        
        qrCodeLabel = new QLabel("QR Code will appear here");
        qrCodeLabel->setAlignment(Qt::AlignCenter);
        qrCodeLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
        qrCodeLabel->setMinimumSize(200, 200);
        
        cryptoAmountLabel = new QLabel("Amount: 0.0 BTC");
        addressLabel = new QLabel("Address: -");
        expiryLabel = new QLabel("Expires: -");
        statusLabel = new QLabel("Status: -");
        
        QFont detailsFont = statusLabel->font();
        detailsFont.setBold(true);
        statusLabel->setFont(detailsFont);
        
        paymentDetailsLayout->addWidget(qrCodeLabel);
        paymentDetailsLayout->addWidget(cryptoAmountLabel);
        paymentDetailsLayout->addWidget(addressLabel);
        paymentDetailsLayout->addWidget(expiryLabel);
        paymentDetailsLayout->addWidget(statusLabel);
        
        layout->addWidget(paymentDetailsWidget);
        paymentDetailsWidget->setVisible(false);
        
        // Back to shopping button (initially hidden)
        backToShoppingButton = new QPushButton("Back to Shopping");
        backToShoppingButton->setVisible(false);
        layout->addWidget(backToShoppingButton);
        
        // Currency label for reference
        currencyLabel = new QLabel(QString("Amount (%1):").arg(selectedCurrency.c_str()));
        
        // Connect signals
        connect(cryptoCurrencyComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCryptoCurrencyChanged(int)));
        connect(payWithCryptoButton, SIGNAL(clicked()), this, SLOT(onPayWithCryptoClicked()));
        connect(cancelPaymentButton, SIGNAL(clicked()), this, SLOT(onCancelPaymentClicked()));
        connect(backToShoppingButton, SIGNAL(clicked()), this, SLOT(onBackToShoppingClicked()));
    }
    
    void startCheckingPaymentStatus(const std::string& transactionId) {
        // In a real implementation, we would check the status periodically
        // For this example, we'll simulate a successful payment after 5 seconds
        
        QTimer::singleShot(5000, [this]() {
            statusLabel->setText("Status: Payment completed successfully!");
            
            // Show success message
            QMessageBox::information(this, "Payment Successful", 
                "Your payment has been received and your order is being processed.");
            
            // Show back to shopping button
            backToShoppingButton->setVisible(true);
            
            // Hide cancel button
            cancelPaymentButton->setVisible(false);
        });
    }
    
    std::string getDefaultCurrencyForCountry(const std::string& countryCode) {
        std::map<std::string, std::string> currencyMap = {
            {"MY", "MYR"}, // Malaysian Ringgit
            {"SG", "SGD"}, // Singapore Dollar
            {"ID", "IDR"}, // Indonesian Rupiah
            {"TH", "THB"}, // Thai Baht
            {"BN", "BND"}, // Brunei Dollar
            {"KH", "KHR"}, // Cambodian Riel
            {"VN", "VND"}, // Vietnamese Dong
            {"LA", "LAK"}  // Lao Kip
        };
        
        auto it = currencyMap.find(countryCode);
        return it != currencyMap.end() ? it->second : "USD";
    }
};

/**
 * Main function to run the kiosk application
 */
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    KioskApplication kiosk;
    kiosk.show();
    
    return app.exec();
}
