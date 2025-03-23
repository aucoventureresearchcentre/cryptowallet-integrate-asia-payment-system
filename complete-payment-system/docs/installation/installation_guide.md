# Asian Cryptocurrency Payment System - Installation Guide

## Overview

This guide provides step-by-step instructions for installing and configuring the Asian Cryptocurrency Payment System across different platforms. The system supports integration with websites, point-of-sale (POS) terminals, and self-service kiosks, allowing merchants to accept cryptocurrency payments throughout Asia.

## Prerequisites

Before installation, ensure you have the following:

1. **Merchant Account**: Register for a merchant account at [asiancryptopay.com](https://asiancryptopay.com)
2. **API Credentials**: Obtain your API key and merchant ID from the merchant dashboard
3. **Regulatory Compliance**: Ensure you have the necessary licenses and registrations for your country of operation
4. **Technical Requirements**: Verify your system meets the minimum technical requirements for your platform

## Web Integration

### Requirements

- Web server with HTTPS support
- Node.js 14.0+ (for npm package) or any web server for direct script inclusion

### Installation Options

#### Option 1: Using NPM

```bash
# Install the SDK package
npm install asian-crypto-payment

# Import in your JavaScript
const AsianCryptoPayment = require('asian-crypto-payment');
```

#### Option 2: Direct Script Inclusion

```html
<!-- Include the SDK directly in your HTML -->
<script src="https://cdn.asiancryptopay.com/sdk/v1/asian-crypto-payment.js"></script>
```

### Configuration

Initialize the SDK with your merchant credentials:

```javascript
const asianCryptoPayment = new AsianCryptoPayment({
    apiKey: 'your_api_key',
    merchantId: 'your_merchant_id',
    countryCode: 'SG', // Country code (MY, SG, ID, TH, BN, KH, VN, LA)
    testMode: false // Set to true for testing
});
```

### Basic Implementation

1. **Create a Payment Button**:

```javascript
// Render a payment button in a container
asianCryptoPayment.renderPaymentButton('payment-button-container', {
    amount: 100.00,
    currency: 'SGD',
    cryptoCurrency: 'BTC',
    description: 'Product purchase'
}, {
    text: 'Pay with Crypto',
    backgroundColor: '#2d54a3',
    textColor: 'white'
});
```

2. **Create a Payment QR Code**:

```javascript
// Render a payment QR code in a container
asianCryptoPayment.renderPaymentQRCode('payment-qr-container', {
    amount: 100.00,
    currency: 'SGD',
    cryptoCurrency: 'BTC',
    description: 'Product purchase'
}, {
    size: 250,
    showStatusChecker: true,
    checkInterval: 5000,
    onSuccess: function(payment) {
        console.log('Payment completed:', payment);
    }
});
```

## POS Terminal Integration

### Requirements

- Android device running Android 6.0 (API level 23) or higher
- Internet connectivity
- Camera for QR code scanning (optional)

### Installation

#### Option 1: Using Gradle

Add the SDK to your app's `build.gradle` file:

```gradle
dependencies {
    implementation 'com.asiancryptopay:sdk:1.0.0'
}
```

#### Option 2: Manual Installation

1. Download the SDK JAR file from [asiancryptopay.com/downloads](https://asiancryptopay.com/downloads)
2. Add the JAR file to your project's `libs` folder
3. Add the dependency in your `build.gradle` file:

```gradle
dependencies {
    implementation files('libs/asian-crypto-payment-sdk.jar')
}
```

### Configuration

Initialize the SDK in your Android application:

```java
import com.asiancryptopay.sdk.AsianCryptoPayment;

// Initialize the SDK
AsianCryptoPayment paymentSDK = new AsianCryptoPayment(
    "your_api_key",
    "your_merchant_id",
    "SG", // Country code (MY, SG, ID, TH, BN, KH, VN, LA)
    "https://api.asiancryptopay.com/v1",
    false, // Set to true for testing
    "SGD", // Default currency
    Arrays.asList("BTC", "ETH", "USDT") // Supported cryptocurrencies
);
```

### Basic Implementation

1. **Create a Payment Button**:

```java
// Create a payment button
Button paymentButton = paymentSDK.createPaymentButton(
    context,
    100.00, // Amount
    "SGD", // Currency
    "BTC", // Cryptocurrency
    "Pay with Crypto", // Button text
    new AsianCryptoPayment.PaymentCallback() {
        @Override
        public void onSuccess(JSONObject payment) {
            // Handle successful payment creation
        }
        
        @Override
        public void onError(Exception e) {
            // Handle error
        }
    }
);

// Add the button to your layout
layout.addView(paymentButton);
```

2. **Create a Payment QR View**:

```java
// Create a payment QR view
LinearLayout qrView = paymentSDK.createPaymentQRView(
    context,
    100.00, // Amount
    "SGD", // Currency
    "BTC", // Cryptocurrency
    new AsianCryptoPayment.PaymentCallback() {
        @Override
        public void onSuccess(JSONObject payment) {
            // Handle successful payment
        }
        
        @Override
        public void onError(Exception e) {
            // Handle error
        }
    }
);

// Add the QR view to your layout
layout.addView(qrView);
```

## Kiosk Integration

### Requirements

- Linux, Windows, or macOS operating system
- C++ development environment
- Qt 5.12+ framework
- Internet connectivity
- Display for QR code presentation

### Installation

#### Option 1: Using CMake

1. Download the SDK from [asiancryptopay.com/downloads](https://asiancryptopay.com/downloads)
2. Extract the files to your project directory
3. Add the SDK to your CMakeLists.txt:

```cmake
# Add the SDK directory
add_subdirectory(asian-crypto-payment-sdk)

# Link against the SDK
target_link_libraries(your_application asian_crypto_payment)
```

#### Option 2: Manual Installation

1. Download the SDK source files
2. Include the header files in your project
3. Link against the required libraries:
   - libcurl
   - jsoncpp
   - Qt5 (Core, Widgets, Network)

### Configuration

Initialize the SDK in your C++ application:

```cpp
#include "asian_crypto_payment.h"

// Initialize the SDK
AsianCryptoPayment* paymentSDK = new AsianCryptoPayment(
    "your_api_key",
    "your_merchant_id",
    "SG", // Country code (MY, SG, ID, TH, BN, KH, VN, LA)
    "https://api.asiancryptopay.com/v1",
    false // Set to true for testing
);
```

### Basic Implementation

1. **Create a Payment UI**:

```cpp
// Create a payment UI
QWidget* paymentUI = paymentSDK->createPaymentUI(
    100.00, // Amount
    "SGD", // Currency
    "BTC", // Cryptocurrency
    parentWidget // Parent widget
);

// Show the payment UI
paymentUI->show();
```

2. **Create a Custom Payment Flow**:

```cpp
try {
    // Create payment
    Json::Value payment = paymentSDK->createPayment(
        100.00, // Amount
        "SGD", // Currency
        "BTC" // Cryptocurrency
    );
    
    // Extract payment details
    std::string paymentAddress = payment["payment_address"].asString();
    std::string transactionId = payment["transaction_id"].asString();
    double cryptoAmount = payment["crypto_amount"].asDouble();
    
    // Display QR code and payment details
    // ...
    
    // Check payment status
    Json::Value status = paymentSDK->getPaymentStatus(transactionId);
    
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
}
```

## Country-Specific Configuration

Each country has specific regulatory requirements. Configure your integration accordingly:

### Malaysia

```javascript
// Web SDK example
const asianCryptoPayment = new AsianCryptoPayment({
    apiKey: 'your_api_key',
    merchantId: 'your_merchant_id',
    countryCode: 'MY',
    scLicenseNumber: 'SC-123456', // Securities Commission license number
    bnmRegistration: 'BNM-789012' // Bank Negara Malaysia registration
});
```

### Singapore

```javascript
// Web SDK example
const asianCryptoPayment = new AsianCryptoPayment({
    apiKey: 'your_api_key',
    merchantId: 'your_merchant_id',
    countryCode: 'SG',
    psaLicenseNumber: 'PSA-123456', // Payment Services Act license number
    travelRuleEnabled: true // Enable Travel Rule compliance
});
```

Similar configurations are available for Indonesia, Thailand, Brunei, Cambodia, Vietnam, and Laos. Refer to the country-specific documentation for details.

## Testing

The system provides a test mode for integration testing:

1. Set `testMode: true` when initializing the SDK
2. Use test API credentials from your merchant dashboard
3. Test transactions will not result in actual cryptocurrency transfers

Test cryptocurrencies are available for each supported blockchain:

- Bitcoin Testnet (tBTC)
- Ethereum Goerli (gETH)
- USDT Testnet (tUSDT)

## Troubleshooting

### Common Issues

1. **API Authentication Errors**:
   - Verify your API key and merchant ID
   - Check that your system clock is synchronized (for request signing)

2. **Payment Creation Failures**:
   - Ensure you're providing valid amount and currency values
   - Verify the cryptocurrency is supported in your country

3. **QR Code Display Issues**:
   - Check that the container element exists in the DOM
   - Verify the container has sufficient dimensions

4. **Payment Status Updates**:
   - Ensure your internet connection is stable
   - Verify the transaction ID is correct

### Support

For additional assistance:

- Documentation: [docs.asiancryptopay.com](https://docs.asiancryptopay.com)
- Support Email: support@asiancryptopay.com
- Developer Forum: [forum.asiancryptopay.com](https://forum.asiancryptopay.com)

## Next Steps

After installation, refer to the following resources:

1. [API Reference](../api/api_reference.md) for detailed API documentation
2. [Regulatory Compliance Guide](../regulatory/compliance_guide.md) for country-specific regulations
3. [Security Best Practices](../security/security_guide.md) for securing your integration
4. [Example Implementations](../../examples/) for working code examples
