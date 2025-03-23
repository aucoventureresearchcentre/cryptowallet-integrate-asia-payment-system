// Asian Cryptocurrency Payment System - Web SDK
// This JavaScript SDK allows websites to integrate with the payment system

/**
 * Main class for the Asian Cryptocurrency Payment System Web SDK
 */
class AsianCryptoPayment {
  /**
   * Initialize the SDK with merchant credentials and configuration
   * @param {Object} config - Configuration options
   * @param {string} config.apiKey - Merchant API key
   * @param {string} config.merchantId - Merchant ID
   * @param {string} config.countryCode - ISO country code (MY, SG, ID, TH, BN, KH, VN, LA)
   * @param {string} config.apiUrl - API endpoint URL (optional)
   * @param {boolean} config.testMode - Whether to use test mode (optional)
   * @param {string} config.defaultCurrency - Default fiat currency (optional)
   * @param {Array<string>} config.supportedCryptocurrencies - Supported cryptocurrencies (optional)
   */
  constructor(config) {
    if (!config.apiKey) throw new Error('API key is required');
    if (!config.merchantId) throw new Error('Merchant ID is required');
    if (!config.countryCode) throw new Error('Country code is required');
    
    this.apiKey = config.apiKey;
    this.merchantId = config.merchantId;
    this.countryCode = config.countryCode.toUpperCase();
    this.apiUrl = config.apiUrl || 'https://api.asiancryptopay.com/v1';
    this.testMode = config.testMode || false;
    this.defaultCurrency = config.defaultCurrency || this._getDefaultCurrencyForCountry();
    this.supportedCryptocurrencies = config.supportedCryptocurrencies || this._getDefaultCryptocurrencies();
    
    // Validate country code
    const supportedCountries = ['MY', 'SG', 'ID', 'TH', 'BN', 'KH', 'VN', 'LA'];
    if (!supportedCountries.includes(this.countryCode)) {
      throw new Error(`Unsupported country code: ${this.countryCode}. Supported countries: ${supportedCountries.join(', ')}`);
    }
  }
  
  /**
   * Create a new payment
   * @param {Object} paymentDetails - Payment details
   * @param {number} paymentDetails.amount - Payment amount in fiat currency
   * @param {string} paymentDetails.currency - Fiat currency code (optional, defaults to defaultCurrency)
   * @param {string} paymentDetails.cryptoCurrency - Cryptocurrency to accept (optional)
   * @param {string} paymentDetails.orderId - Merchant order ID (optional)
   * @param {string} paymentDetails.description - Payment description (optional)
   * @param {string} paymentDetails.callbackUrl - Webhook URL for payment notifications (optional)
   * @param {string} paymentDetails.successUrl - URL to redirect after successful payment (optional)
   * @param {string} paymentDetails.cancelUrl - URL to redirect after cancelled payment (optional)
   * @param {Object} paymentDetails.metadata - Additional metadata (optional)
   * @returns {Promise<Object>} Payment details including payment URL and address
   */
  async createPayment(paymentDetails) {
    if (!paymentDetails.amount || paymentDetails.amount <= 0) {
      throw new Error('Valid payment amount is required');
    }
    
    const currency = paymentDetails.currency || this.defaultCurrency;
    const cryptoCurrency = paymentDetails.cryptoCurrency || this.supportedCryptocurrencies[0];
    
    // Validate cryptocurrency is supported
    if (!this.supportedCryptocurrencies.includes(cryptoCurrency)) {
      throw new Error(`Unsupported cryptocurrency: ${cryptoCurrency}. Supported: ${this.supportedCryptocurrencies.join(', ')}`);
    }
    
    const payload = {
      amount: paymentDetails.amount,
      currency: currency,
      crypto_currency: cryptoCurrency,
      merchant_id: this.merchantId,
      country_code: this.countryCode,
      order_id: paymentDetails.orderId,
      description: paymentDetails.description,
      callback_url: paymentDetails.callbackUrl,
      success_url: paymentDetails.successUrl,
      cancel_url: paymentDetails.cancelUrl,
      metadata: paymentDetails.metadata || {},
      test_mode: this.testMode
    };
    
    try {
      const response = await this._makeApiRequest('/payments', 'POST', payload);
      return response;
    } catch (error) {
      throw new Error(`Failed to create payment: ${error.message}`);
    }
  }
  
  /**
   * Get payment status
   * @param {string} paymentId - Payment ID
   * @returns {Promise<Object>} Payment status details
   */
  async getPaymentStatus(paymentId) {
    if (!paymentId) throw new Error('Payment ID is required');
    
    try {
      const response = await this._makeApiRequest(`/payments/${paymentId}`, 'GET');
      return response;
    } catch (error) {
      throw new Error(`Failed to get payment status: ${error.message}`);
    }
  }
  
  /**
   * Get exchange rates
   * @param {string} fiatCurrency - Fiat currency code
   * @param {string} cryptoCurrency - Cryptocurrency code
   * @returns {Promise<Object>} Exchange rate details
   */
  async getExchangeRate(fiatCurrency, cryptoCurrency) {
    if (!fiatCurrency) throw new Error('Fiat currency is required');
    if (!cryptoCurrency) throw new Error('Cryptocurrency is required');
    
    try {
      const response = await this._makeApiRequest(`/exchange-rates?fiat=${fiatCurrency}&crypto=${cryptoCurrency}`, 'GET');
      return response;
    } catch (error) {
      throw new Error(`Failed to get exchange rate: ${error.message}`);
    }
  }
  
  /**
   * Get supported cryptocurrencies for the merchant's country
   * @returns {Promise<Array<string>>} List of supported cryptocurrencies
   */
  async getSupportedCryptocurrencies() {
    try {
      const response = await this._makeApiRequest(`/cryptocurrencies?country=${this.countryCode}`, 'GET');
      return response.cryptocurrencies;
    } catch (error) {
      throw new Error(`Failed to get supported cryptocurrencies: ${error.message}`);
    }
  }
  
  /**
   * Get regulatory information for the merchant's country
   * @returns {Promise<Object>} Regulatory information
   */
  async getRegulatoryInfo() {
    try {
      const response = await this._makeApiRequest(`/regulatory-info?country=${this.countryCode}`, 'GET');
      return response;
    } catch (error) {
      throw new Error(`Failed to get regulatory information: ${error.message}`);
    }
  }
  
  /**
   * Render payment button
   * @param {string} elementId - ID of the HTML element to render the button in
   * @param {Object} paymentDetails - Payment details
   * @param {Object} buttonOptions - Button customization options
   * @returns {HTMLElement} The created button element
   */
  renderPaymentButton(elementId, paymentDetails, buttonOptions = {}) {
    const container = document.getElementById(elementId);
    if (!container) throw new Error(`Element with ID "${elementId}" not found`);
    
    const button = document.createElement('button');
    button.className = 'asian-crypto-pay-button';
    button.innerText = buttonOptions.text || 'Pay with Cryptocurrency';
    
    // Apply custom styles
    if (buttonOptions.backgroundColor) button.style.backgroundColor = buttonOptions.backgroundColor;
    if (buttonOptions.textColor) button.style.color = buttonOptions.textColor;
    if (buttonOptions.borderRadius) button.style.borderRadius = buttonOptions.borderRadius;
    if (buttonOptions.padding) button.style.padding = buttonOptions.padding;
    if (buttonOptions.fontSize) button.style.fontSize = buttonOptions.fontSize;
    
    // Default styles
    button.style.cursor = 'pointer';
    button.style.border = 'none';
    button.style.padding = buttonOptions.padding || '12px 24px';
    button.style.fontWeight = 'bold';
    button.style.backgroundColor = buttonOptions.backgroundColor || '#2d54a3';
    button.style.color = buttonOptions.textColor || 'white';
    button.style.borderRadius = buttonOptions.borderRadius || '4px';
    
    // Add click handler
    button.addEventListener('click', async () => {
      try {
        const payment = await this.createPayment(paymentDetails);
        
        // Open payment page in new window or redirect
        if (buttonOptions.openInNewWindow) {
          window.open(payment.payment_url, '_blank');
        } else {
          window.location.href = payment.payment_url;
        }
      } catch (error) {
        console.error('Payment creation failed:', error);
        if (buttonOptions.onError) buttonOptions.onError(error);
      }
    });
    
    container.appendChild(button);
    return button;
  }
  
  /**
   * Render payment QR code
   * @param {string} elementId - ID of the HTML element to render the QR code in
   * @param {Object} paymentDetails - Payment details
   * @param {Object} qrOptions - QR code customization options
   * @returns {Promise<HTMLElement>} The created QR code element
   */
  async renderPaymentQRCode(elementId, paymentDetails, qrOptions = {}) {
    const container = document.getElementById(elementId);
    if (!container) throw new Error(`Element with ID "${elementId}" not found`);
    
    try {
      const payment = await this.createPayment(paymentDetails);
      
      const qrContainer = document.createElement('div');
      qrContainer.className = 'asian-crypto-pay-qr';
      
      // Create QR code
      const qrCode = document.createElement('img');
      qrCode.src = `https://chart.googleapis.com/chart?cht=qr&chs=${qrOptions.size || 250}x${qrOptions.size || 250}&chl=${encodeURIComponent(payment.payment_address)}`;
      qrCode.alt = 'Payment QR Code';
      qrContainer.appendChild(qrCode);
      
      // Add payment details
      const details = document.createElement('div');
      details.className = 'asian-crypto-pay-details';
      details.innerHTML = `
        <p><strong>Amount:</strong> ${payment.crypto_amount} ${payment.crypto_currency}</p>
        <p><strong>Address:</strong> ${payment.payment_address}</p>
        <p><strong>Expires:</strong> ${new Date(payment.expires_at).toLocaleString()}</p>
      `;
      qrContainer.appendChild(details);
      
      // Add status checker
      if (qrOptions.showStatusChecker !== false) {
        const statusChecker = document.createElement('div');
        statusChecker.className = 'asian-crypto-pay-status';
        
        const statusText = document.createElement('p');
        statusText.innerText = 'Waiting for payment...';
        statusChecker.appendChild(statusText);
        
        // Check status periodically
        const checkInterval = setInterval(async () => {
          try {
            const status = await this.getPaymentStatus(payment.transaction_id);
            statusText.innerText = `Status: ${status.status}`;
            
            if (status.status === 'completed') {
              clearInterval(checkInterval);
              statusText.innerText = 'Payment completed!';
              statusText.style.color = 'green';
              
              if (qrOptions.onSuccess) qrOptions.onSuccess(status);
              if (paymentDetails.successUrl) {
                setTimeout(() => {
                  window.location.href = paymentDetails.successUrl;
                }, 2000);
              }
            } else if (status.status === 'failed') {
              clearInterval(checkInterval);
              statusText.innerText = 'Payment failed!';
              statusText.style.color = 'red';
              
              if (qrOptions.onError) qrOptions.onError(status);
            } else if (new Date() > new Date(payment.expires_at)) {
              clearInterval(checkInterval);
              statusText.innerText = 'Payment expired!';
              statusText.style.color = 'red';
              
              if (qrOptions.onExpire) qrOptions.onExpire(status);
            }
          } catch (error) {
            console.error('Status check failed:', error);
          }
        }, qrOptions.checkInterval || 5000);
        
        qrContainer.appendChild(statusChecker);
      }
      
      container.appendChild(qrContainer);
      return qrContainer;
    } catch (error) {
      console.error('QR code creation failed:', error);
      if (qrOptions.onError) qrOptions.onError(error);
      throw error;
    }
  }
  
  /**
   * Make API request to the payment system
   * @private
   * @param {string} endpoint - API endpoint
   * @param {string} method - HTTP method
   * @param {Object} data - Request data
   * @returns {Promise<Object>} Response data
   */
  async _makeApiRequest(endpoint, method, data = null) {
    const url = `${this.apiUrl}${endpoint}`;
    const timestamp = Date.now();
    const nonce = this._generateNonce();
    
    const headers = {
      'Content-Type': 'application/json',
      'X-API-Key': this.apiKey,
      'X-Timestamp': timestamp.toString(),
      'X-Nonce': nonce
    };
    
    // Add signature for authentication
    if (data) {
      const signature = this._generateSignature(endpoint, method, timestamp, nonce, data);
      headers['X-Signature'] = signature;
    }
    
    const options = {
      method,
      headers,
      body: data ? JSON.stringify(data) : undefined
    };
    
    const response = await fetch(url, options);
    const responseData = await response.json();
    
    if (!response.ok) {
      throw new Error(responseData.error || `API request failed with status ${response.status}`);
    }
    
    return responseData;
  }
  
  /**
   * Generate a random nonce
   * @private
   * @returns {string} Random nonce
   */
  _generateNonce() {
    return Math.random().toString(36).substring(2, 15) + Math.random().toString(36).substring(2, 15);
  }
  
  /**
   * Generate signature for API authentication
   * @private
   * @param {string} endpoint - API endpoint
   * @param {string} method - HTTP method
   * @param {number} timestamp - Request timestamp
   * @param {string} nonce - Random nonce
   * @param {Object} data - Request data
   * @returns {string} HMAC signature
   */
  _generateSignature(endpoint, method, timestamp, nonce, data) {
    // In a real implementation, this would use a crypto library to generate an HMAC
    // For this example, we'll return a placeholder
    return 'signature_placeholder';
  }
  
  /**
   * Get default currency for country
   * @private
   * @returns {string} Default currency code
   */
  _getDefaultCurrencyForCountry() {
    const currencyMap = {
      'MY': 'MYR', // Malaysian Ringgit
      'SG': 'SGD', // Singapore Dollar
      'ID': 'IDR', // Indonesian Rupiah
      'TH': 'THB', // Thai Baht
      'BN': 'BND', // Brunei Dollar
      'KH': 'KHR', // Cambodian Riel
      'VN': 'VND', // Vietnamese Dong
      'LA': 'LAK'  // Lao Kip
    };
    
    return currencyMap[this.countryCode] || 'USD';
  }
  
  /**
   * Get default cryptocurrencies
   * @private
   * @returns {Array<string>} Default supported cryptocurrencies
   */
  _getDefaultCryptocurrencies() {
    // Common cryptocurrencies supported across most countries
    return ['BTC', 'ETH', 'USDT'];
  }
}

// Export for CommonJS and ES modules
if (typeof module !== 'undefined' && typeof module.exports !== 'undefined') {
  module.exports = AsianCryptoPayment;
} else {
  window.AsianCryptoPayment = AsianCryptoPayment;
}
