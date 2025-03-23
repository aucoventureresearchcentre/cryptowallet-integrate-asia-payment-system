/**
 * Asian Cryptocurrency Payment System - Web SDK
 * Version: 1.0.0
 * 
 * This SDK provides a comprehensive interface for integrating cryptocurrency
 * payments into websites across Asian markets, with support for regulatory
 * compliance in Malaysia, Singapore, Indonesia, Thailand, Brunei, Cambodia,
 * Vietnam, and Laos.
 */

class AsianCryptoPayment {
  /**
   * Initialize the SDK with merchant credentials and configuration
   * 
   * @param {Object} config - Configuration object
   * @param {string} config.apiKey - Merchant API key
   * @param {string} config.merchantId - Merchant ID
   * @param {string} config.countryCode - Two-letter country code (MY, SG, ID, TH, BN, KH, VN, LA)
   * @param {boolean} [config.testMode=false] - Whether to use test mode
   * @param {string} [config.apiEndpoint] - Custom API endpoint (optional)
   * @param {Object} [config.webhookConfig] - Webhook configuration (optional)
   * @param {string[]} [config.supportedCryptocurrencies] - List of supported cryptocurrencies (optional)
   */
  constructor(config) {
    // Validate required parameters
    if (!config.apiKey) throw new Error('API key is required');
    if (!config.merchantId) throw new Error('Merchant ID is required');
    if (!config.countryCode) throw new Error('Country code is required');
    
    // Validate country code
    const validCountryCodes = ['MY', 'SG', 'ID', 'TH', 'BN', 'KH', 'VN', 'LA'];
    if (!validCountryCodes.includes(config.countryCode)) {
      throw new Error(`Invalid country code. Must be one of: ${validCountryCodes.join(', ')}`);
    }
    
    // Initialize configuration
    this.config = {
      apiKey: config.apiKey,
      merchantId: config.merchantId,
      countryCode: config.countryCode,
      testMode: config.testMode || false,
      apiEndpoint: config.apiEndpoint || 'https://api.asiancryptopay.com',
      webhookConfig: config.webhookConfig || null,
      supportedCryptocurrencies: config.supportedCryptocurrencies || [
        'BTC', 'ETH', 'USDT', 'USDC', 'BNB'
      ]
    };
    
    // Initialize state
    this.state = {
      initialized: true,
      lastResponse: null,
      activePayments: new Map()
    };
    
    // Initialize country-specific module
    this._initializeCountryModule();
    
    // Initialize security module
    this._initializeSecurity();
    
    console.log(`Asian Cryptocurrency Payment SDK initialized for ${this.config.countryCode}`);
  }
  
  /**
   * Initialize country-specific regulatory compliance module
   * @private
   */
  _initializeCountryModule() {
    // Map country codes to regulatory modules
    const countryModules = {
      'MY': MalaysiaComplianceModule,
      'SG': SingaporeComplianceModule,
      'ID': IndonesiaComplianceModule,
      'TH': ThailandComplianceModule,
      'BN': BruneiComplianceModule,
      'KH': CambodiaComplianceModule,
      'VN': VietnamComplianceModule,
      'LA': LaosComplianceModule
    };
    
    // Initialize the appropriate module
    const CountryModule = countryModules[this.config.countryCode];
    this.countryModule = new CountryModule(this.config);
    
    // Apply country-specific configurations
    this.countryModule.applyRegulations();
  }
  
  /**
   * Initialize security features
   * @private
   */
  _initializeSecurity() {
    this.security = {
      encryptData: (data) => {
        // Implementation of AES-256 encryption
        return btoa(JSON.stringify(data)); // Simplified for example
      },
      decryptData: (encryptedData) => {
        // Implementation of AES-256 decryption
        return JSON.parse(atob(encryptedData)); // Simplified for example
      },
      generateSignature: (payload, timestamp) => {
        // Implementation of HMAC-SHA256 signature
        const message = `${timestamp}.${JSON.stringify(payload)}`;
        // In a real implementation, use a proper HMAC library
        return this._sha256(this.config.apiKey + message);
      },
      verifySignature: (signature, payload, timestamp) => {
        const expectedSignature = this.security.generateSignature(payload, timestamp);
        return signature === expectedSignature;
      }
    };
  }
  
  /**
   * Simple SHA-256 implementation for example purposes
   * In production, use a proper crypto library
   * @private
   */
  _sha256(message) {
    // This is a placeholder - in production use a proper crypto library
    let hash = 0;
    for (let i = 0; i < message.length; i++) {
      const char = message.charCodeAt(i);
      hash = ((hash << 5) - hash) + char;
      hash = hash & hash;
    }
    return hash.toString(16);
  }
  
  /**
   * Make an API request to the backend
   * @private
   * @param {string} endpoint - API endpoint
   * @param {string} method - HTTP method
   * @param {Object} [data] - Request payload
   * @returns {Promise<Object>} - API response
   */
  async _apiRequest(endpoint, method, data = null) {
    const url = `${this.config.apiEndpoint}/${endpoint}`;
    const timestamp = Date.now().toString();
    
    const headers = {
      'Content-Type': 'application/json',
      'X-Merchant-ID': this.config.merchantId,
      'X-Timestamp': timestamp,
      'X-Test-Mode': this.config.testMode ? 'true' : 'false'
    };
    
    if (data) {
      headers['X-Signature'] = this.security.generateSignature(data, timestamp);
    }
    
    const requestOptions = {
      method,
      headers,
      body: data ? JSON.stringify(data) : undefined
    };
    
    try {
      const response = await fetch(url, requestOptions);
      const responseData = await response.json();
      
      // Store last response for debugging
      this.state.lastResponse = responseData;
      
      if (!response.ok) {
        throw new Error(responseData.message || 'API request failed');
      }
      
      return responseData;
    } catch (error) {
      console.error('API request failed:', error);
      throw error;
    }
  }
  
  /**
   * Create a new cryptocurrency payment
   * 
   * @param {Object} paymentDetails - Payment details
   * @param {number} paymentDetails.amount - Payment amount
   * @param {string} paymentDetails.currency - Fiat currency code (e.g., MYR, SGD)
   * @param {string} paymentDetails.cryptoCurrency - Cryptocurrency code (e.g., BTC, ETH)
   * @param {string} [paymentDetails.description] - Payment description
   * @param {string} [paymentDetails.orderId] - Merchant order ID
   * @param {string} [paymentDetails.customerEmail] - Customer email
   * @param {string} [paymentDetails.customerName] - Customer name
   * @param {string} [paymentDetails.callbackUrl] - Callback URL for payment updates
   * @param {string} [paymentDetails.successUrl] - Redirect URL on successful payment
   * @param {string} [paymentDetails.cancelUrl] - Redirect URL on cancelled payment
   * @param {Object} [paymentDetails.metadata] - Additional metadata
   * @returns {Promise<Object>} - Payment object
   */
  async createPayment(paymentDetails) {
    // Validate required parameters
    if (!paymentDetails.amount) throw new Error('Payment amount is required');
    if (!paymentDetails.currency) throw new Error('Currency is required');
    if (!paymentDetails.cryptoCurrency) throw new Error('Cryptocurrency is required');
    
    // Validate amount
    if (paymentDetails.amount <= 0) {
      throw new Error('Payment amount must be greater than zero');
    }
    
    // Validate cryptocurrency
    if (!this.config.supportedCryptocurrencies.includes(paymentDetails.cryptoCurrency)) {
      throw new Error(`Unsupported cryptocurrency. Must be one of: ${this.config.supportedCryptocurrencies.join(', ')}`);
    }
    
    // Apply country-specific validations
    this.countryModule.validatePayment(paymentDetails);
    
    // Prepare payment data
    const paymentData = {
      merchant_id: this.config.merchantId,
      amount: paymentDetails.amount,
      currency: paymentDetails.currency,
      crypto_currency: paymentDetails.cryptoCurrency,
      description: paymentDetails.description || '',
      order_id: paymentDetails.orderId || `order-${Date.now()}`,
      customer_email: paymentDetails.customerEmail || '',
      customer_name: paymentDetails.customerName || '',
      callback_url: paymentDetails.callbackUrl || '',
      success_url: paymentDetails.successUrl || '',
      cancel_url: paymentDetails.cancelUrl || '',
      metadata: paymentDetails.metadata || {},
      country_code: this.config.countryCode,
      test_mode: this.config.testMode
    };
    
    // Make API request
    const payment = await this._apiRequest('payments', 'POST', paymentData);
    
    // Store active payment
    this.state.activePayments.set(payment.id, payment);
    
    return payment;
  }
  
  /**
   * Get payment details by ID
   * 
   * @param {string} paymentId - Payment ID
   * @returns {Promise<Object>} - Payment object
   */
  async getPayment(paymentId) {
    if (!paymentId) throw new Error('Payment ID is required');
    
    const payment = await this._apiRequest(`payments/${paymentId}`, 'GET');
    
    // Update active payment if exists
    if (this.state.activePayments.has(paymentId)) {
      this.state.activePayments.set(paymentId, payment);
    }
    
    return payment;
  }
  
  /**
   * Get list of payments
   * 
   * @param {Object} [filters] - Filter parameters
   * @param {string} [filters.status] - Filter by status
   * @param {string} [filters.fromDate] - Filter by date range start
   * @param {string} [filters.toDate] - Filter by date range end
   * @param {number} [filters.limit=20] - Number of results to return
   * @param {number} [filters.offset=0] - Offset for pagination
   * @returns {Promise<Object>} - List of payments
   */
  async getPayments(filters = {}) {
    const queryParams = new URLSearchParams();
    
    if (filters.status) queryParams.append('status', filters.status);
    if (filters.fromDate) queryParams.append('from_date', filters.fromDate);
    if (filters.toDate) queryParams.append('to_date', filters.toDate);
    if (filters.limit) queryParams.append('limit', filters.limit.toString());
    if (filters.offset) queryParams.append('offset', filters.offset.toString());
    
    const endpoint = `payments?${queryParams.toString()}`;
    return await this._apiRequest(endpoint, 'GET');
  }
  
  /**
   * Cancel a payment
   * 
   * @param {string} paymentId - Payment ID
   * @returns {Promise<Object>} - Cancelled payment object
   */
  async cancelPayment(paymentId) {
    if (!paymentId) throw new Error('Payment ID is required');
    
    const payment = await this._apiRequest(`payments/${paymentId}/cancel`, 'POST');
    
    // Update active payment if exists
    if (this.state.activePayments.has(paymentId)) {
      this.state.activePayments.set(paymentId, payment);
    }
    
    return payment;
  }
  
  /**
   * Get current exchange rates
   * 
   * @param {string} [baseCurrency='USD'] - Base currency
   * @param {string[]} [cryptoCurrencies] - List of cryptocurrencies to get rates for
   * @returns {Promise<Object>} - Exchange rates
   */
  async getExchangeRates(baseCurrency = 'USD', cryptoCurrencies = null) {
    const currencies = cryptoCurrencies || this.config.supportedCryptocurrencies;
    
    const queryParams = new URLSearchParams();
    queryParams.append('base_currency', baseCurrency);
    queryParams.append('currencies', currencies.join(','));
    
    const endpoint = `exchange-rates?${queryParams.toString()}`;
    return await this._apiRequest(endpoint, 'GET');
  }
  
  /**
   * Create a payment button that can be added to a webpage
   * 
   * @param {string|HTMLElement} container - Container element or selector
   * @param {Object} paymentDetails - Payment details
   * @param {Object} [options] - Button options
   * @param {string} [options.buttonText='Pay with Cryptocurrency'] - Button text
   * @param {string} [options.buttonClass='asian-crypto-pay-button'] - Button CSS class
   * @param {string} [options.theme='light'] - Button theme (light or dark)
   * @param {Function} [options.onSuccess] - Success callback
   * @param {Function} [options.onCancel] - Cancel callback
   * @param {Function} [options.onError] - Error callback
   * @returns {HTMLElement} - Button element
   */
  createPaymentButton(container, paymentDetails, options = {}) {
    // Default options
    const defaultOptions = {
      buttonText: 'Pay with Cryptocurrency',
      buttonClass: 'asian-crypto-pay-button',
      theme: 'light',
      onSuccess: () => {},
      onCancel: () => {},
      onError: () => {}
    };
    
    // Merge options
    const buttonOptions = { ...defaultOptions, ...options };
    
    // Get container element
    const containerElement = typeof container === 'string' 
      ? document.querySelector(container) 
      : container;
    
    if (!containerElement) {
      throw new Error('Container element not found');
    }
    
    // Create button element
    const button = document.createElement('button');
    button.textContent = buttonOptions.buttonText;
    button.className = buttonOptions.buttonClass;
    button.dataset.theme = buttonOptions.theme;
    
    // Add click event listener
    button.addEventListener('click', async () => {
      try {
        // Create payment
        const payment = await this.createPayment(paymentDetails);
        
        // Open payment modal
        this._openPaymentModal(payment, buttonOptions);
      } catch (error) {
        console.error('Payment creation failed:', error);
        buttonOptions.onError(error);
      }
    });
    
    // Add button to container
    containerElement.appendChild(button);
    
    return button;
  }
  
  /**
   * Open payment modal
   * @private
   * @param {Object} payment - Payment object
   * @param {Object} options - Modal options
   */
  _openPaymentModal(payment, options) {
    // Create modal container
    const modal = document.createElement('div');
    modal.className = 'asian-crypto-pay-modal';
    modal.dataset.theme = options.theme;
    
    // Create modal content
    const modalContent = document.createElement('div');
    modalContent.className = 'asian-crypto-pay-modal-content';
    
    // Create modal header
    const modalHeader = document.createElement('div');
    modalHeader.className = 'asian-crypto-pay-modal-header';
    
    const modalTitle = document.createElement('h3');
    modalTitle.textContent = 'Cryptocurrency Payment';
    
    const closeButton = document.createElement('button');
    closeButton.className = 'asian-crypto-pay-modal-close';
    closeButton.textContent = '×';
    closeButton.addEventListener('click', () => {
      document.body.removeChild(modal);
      options.onCancel();
    });
    
    modalHeader.appendChild(modalTitle);
    modalHeader.appendChild(closeButton);
    
    // Create modal body
    const modalBody = document.createElement('div');
    modalBody.className = 'asian-crypto-pay-modal-body';
    
    const paymentInfo = document.createElement('div');
    paymentInfo.className = 'asian-crypto-pay-payment-info';
    
    const amountInfo = document.createElement('p');
    amountInfo.innerHTML = `<strong>Amount:</strong> ${payment.crypto_amount} ${payment.crypto_currency}`;
    
    const addressInfo = document.createElement('p');
    addressInfo.innerHTML = `<strong>Send to address:</strong>`;
    
    const addressValue = document.createElement('div');
    addressValue.className = 'asian-crypto-pay-address';
    addressValue.textContent = payment.address;
    
    const copyButton = document.createElement('button');
    copyButton.className = 'asian-crypto-pay-copy-button';
    copyButton.textContent = 'Copy';
    copyButton.addEventListener('click', () => {
      navigator.clipboard.writeText(payment.address);
      copyButton.textContent = 'Copied!';
      setTimeout(() => {
        copyButton.textContent = 'Copy';
      }, 2000);
    });
    
    addressInfo.appendChild(addressValue);
    addressInfo.appendChild(copyButton);
    
    const qrCodeContainer = document.createElement('div');
    qrCodeContainer.className = 'asian-crypto-pay-qr-code';
    
    // Create QR code (in a real implementation, use a QR code library)
    const qrCode = document.createElement('img');
    qrCode.src = payment.qr_code_url;
    qrCode.alt = 'Payment QR Code';
    
    qrCodeContainer.appendChild(qrCode);
    
    const statusContainer = document.createElement('div');
    statusContainer.className = 'asian-crypto-pay-status';
    statusContainer.innerHTML = `<strong>Status:</strong> <span>${payment.status}</span>`;
    
    paymentInfo.appendChild(amountInfo);
    paymentInfo.appendChild(addressInfo);
    paymentInfo.appendChild(qrCodeContainer);
    paymentInfo.appendChild(statusContainer);
    
    modalBody.appendChild(paymentInfo);
    
    // Create modal footer
    const modalFooter = document.createElement('div');
    modalFooter.className = 'asian-crypto-pay-modal-footer';
    
    const cancelButton = document.createElement('button');
    cancelButton.className = 'asian-crypto-pay-cancel-button';
    cancelButton.textContent = 'Cancel Payment';
    cancelButton.addEventListener('click', async () => {
      try {
        await this.cancelPayment(payment.id);
        document.body.removeChild(modal);
        options.onCancel();
      } catch (error) {
        console.error('Payment cancellation failed:', error);
      }
    });
    
    modalFooter.appendChild(cancelButton);
    
    // Assemble modal
    modalContent.appendChild(modalHeader);
    modalContent.appendChild(modalBody);
    modalContent.appendChild(modalFooter);
    modal.appendChild(modalContent);
    
    // Add modal to body
    document.body.appendChild(modal);
    
    // Start polling for payment status
    this._pollPaymentStatus(payment.id, statusContainer.querySelector('span'), modal, options);
  }
  
  /**
   * Poll payment status
   * @private
   * @param {string} paymentId - Payment ID
   * @param {HTMLElement} statusElement - Status element to update
   * @param {HTMLElement} modal - Modal element
   * @param {Object} options - Modal options
   */
  _pollPaymentStatus(paymentId, statusElement, modal, options) {
    const pollInterval = setInterval(async () => {
      try {
        const payment = await this.getPayment(paymentId);
        statusElement.textContent = payment.status;
        
        if (payment.status === 'completed') {
          clearInterval(pollInterval);
          
          // Show success message
          const modalBody = modal.querySelector('.asian-crypto-pay-modal-body');
          modalBody.innerHTML = `
            <div class="asian-crypto-pay-success">
              <div class="asian-crypto-pay-success-icon">✓</div>
              <h3>Payment Successful!</h3>
              <p>Thank you for your payment.</p>
            </div>
          `;
          
          // Update footer
          const modalFooter = modal.querySelector('.asian-crypto-pay-modal-footer');
          modalFooter.innerHTML = `
            <button class="asian-crypto-pay-close-button">Close</button>
          `;
          
          const closeButton = modalFooter.querySelector('button');
          closeButton.addEventListener('click', () => {
            document.body.removeChild(modal);
            options.onSuccess(payment);
          });
          
          // Auto-close after 5 seconds
          setTimeout(() => {
            if (document.body.contains(modal)) {
              document.body.removeChild(modal);
              options.onSuccess(payment);
            }
          }, 5000);
        } else if (payment.status === 'expired' || payment.status === 'cancelled') {
          clearInterval(pollInterval);
          document.body.removeChild(modal);
          options.onCancel(payment);
        }
      } catch (error) {
        console.error('Payment status check failed:', error);
      }
    }, 5000); // Check every 5 seconds
    
    // Store interval ID to clear it if modal is closed
    modal.dataset.pollInterval = pollInterval;
    
    // Clear interval when modal is closed
    const closeButton = modal.querySelector('.asian-crypto-pay-modal-close');
    closeButton.addEventListener('click', () => {
      clearInterval(pollInterval);
    });
  }
  
  /**
   * Create a payment form that can be added to a webpage
   * 
   * @param {string|HTMLElement} container - Container element or selector
   * @param {Object} [options] - Form options
   * @param {string} [options.formClass='asian-crypto-pay-form'] - Form CSS class
   * @param {string} [options.theme='light'] - Form theme (light or dark)
   * @param {string[]} [options.cryptoCurrencies] - List of cryptocurrencies to offer
   * @param {Function} [options.onSuccess] - Success callback
   * @param {Function} [options.onCancel] - Cancel callback
   * @param {Function} [options.onError] - Error callback
   * @returns {HTMLElement} - Form element
   */
  createPaymentForm(container, options = {}) {
    // Default options
    const defaultOptions = {
      formClass: 'asian-crypto-pay-form',
      theme: 'light',
      cryptoCurrencies: this.config.supportedCryptocurrencies,
      onSuccess: () => {},
      onCancel: () => {},
      onError: () => {}
    };
    
    // Merge options
    const formOptions = { ...defaultOptions, ...options };
    
    // Get container element
    const containerElement = typeof container === 'string' 
      ? document.querySelector(container) 
      : container;
    
    if (!containerElement) {
      throw new Error('Container element not found');
    }
    
    // Create form element
    const form = document.createElement('form');
    form.className = formOptions.formClass;
    form.dataset.theme = formOptions.theme;
    
    // Create form fields
    form.innerHTML = `
      <div class="asian-crypto-pay-form-group">
        <label for="asian-crypto-pay-amount">Amount</label>
        <input type="number" id="asian-crypto-pay-amount" name="amount" required min="0.01" step="0.01">
      </div>
      
      <div class="asian-crypto-pay-form-group">
        <label for="asian-crypto-pay-currency">Currency</label>
        <select id="asian-crypto-pay-currency" name="currency" required>
          <option value="MYR">Malaysian Ringgit (MYR)</option>
          <option value="SGD">Singapore Dollar (SGD)</option>
          <option value="IDR">Indonesian Rupiah (IDR)</option>
          <option value="THB">Thai Baht (THB)</option>
          <option value="BND">Brunei Dollar (BND)</option>
          <option value="KHR">Cambodian Riel (KHR)</option>
          <option value="VND">Vietnamese Dong (VND)</option>
          <option value="LAK">Lao Kip (LAK)</option>
        </select>
      </div>
      
      <div class="asian-crypto-pay-form-group">
        <label for="asian-crypto-pay-crypto-currency">Cryptocurrency</label>
        <select id="asian-crypto-pay-crypto-currency" name="cryptoCurrency" required>
          ${formOptions.cryptoCurrencies.map(crypto => `
            <option value="${crypto}">${crypto}</option>
          `).join('')}
        </select>
      </div>
      
      <div class="asian-crypto-pay-form-group">
        <label for="asian-crypto-pay-description">Description</label>
        <input type="text" id="asian-crypto-pay-description" name="description">
      </div>
      
      <div class="asian-crypto-pay-form-group">
        <label for="asian-crypto-pay-customer-email">Email</label>
        <input type="email" id="asian-crypto-pay-customer-email" name="customerEmail">
      </div>
      
      <div class="asian-crypto-pay-form-group">
        <label for="asian-crypto-pay-customer-name">Name</label>
        <input type="text" id="asian-crypto-pay-customer-name" name="customerName">
      </div>
      
      <div class="asian-crypto-pay-form-actions">
        <button type="submit" class="asian-crypto-pay-submit-button">Pay Now</button>
      </div>
    `;
    
    // Add submit event listener
    form.addEventListener('submit', async (event) => {
      event.preventDefault();
      
      try {
        // Get form data
        const formData = new FormData(form);
        const paymentDetails = {
          amount: parseFloat(formData.get('amount')),
          currency: formData.get('currency'),
          cryptoCurrency: formData.get('cryptoCurrency'),
          description: formData.get('description'),
          customerEmail: formData.get('customerEmail'),
          customerName: formData.get('customerName')
        };
        
        // Create payment
        const payment = await this.createPayment(paymentDetails);
        
        // Open payment modal
        this._openPaymentModal(payment, formOptions);
      } catch (error) {
        console.error('Payment creation failed:', error);
        formOptions.onError(error);
      }
    });
    
    // Add form to container
    containerElement.appendChild(form);
    
    return form;
  }
  
  /**
   * Add CSS styles for payment UI components
   */
  addStyles() {
    // Check if styles already added
    if (document.getElementById('asian-crypto-pay-styles')) {
      return;
    }
    
    // Create style element
    const style = document.createElement('style');
    style.id = 'asian-crypto-pay-styles';
    
    // Add CSS
    style.textContent = `
      /* Button styles */
      .asian-crypto-pay-button {
        display: inline-block;
        padding: 12px 24px;
        background-color: #2d5afe;
        color: white;
        border: none;
        border-radius: 4px;
        font-size: 16px;
        font-weight: 600;
        cursor: pointer;
        transition: background-color 0.2s;
      }
      
      .asian-crypto-pay-button:hover {
        background-color: #0039cb;
      }
      
      .asian-crypto-pay-button[data-theme="dark"] {
        background-color: #5f7aff;
      }
      
      .asian-crypto-pay-button[data-theme="dark"]:hover {
        background-color: #8c9eff;
      }
      
      /* Modal styles */
      .asian-crypto-pay-modal {
        position: fixed;
        top: 0;
        left: 0;
        width: 100%;
        height: 100%;
        background-color: rgba(0, 0, 0, 0.5);
        display: flex;
        justify-content: center;
        align-items: center;
        z-index: 9999;
      }
      
      .asian-crypto-pay-modal-content {
        background-color: white;
        border-radius: 8px;
        width: 90%;
        max-width: 500px;
        max-height: 90vh;
        overflow-y: auto;
        box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
      }
      
      .asian-crypto-pay-modal[data-theme="dark"] .asian-crypto-pay-modal-content {
        background-color: #2c2c2c;
        color: white;
      }
      
      .asian-crypto-pay-modal-header {
        display: flex;
        justify-content: space-between;
        align-items: center;
        padding: 16px;
        border-bottom: 1px solid #eee;
      }
      
      .asian-crypto-pay-modal[data-theme="dark"] .asian-crypto-pay-modal-header {
        border-bottom-color: #444;
      }
      
      .asian-crypto-pay-modal-header h3 {
        margin: 0;
        font-size: 18px;
      }
      
      .asian-crypto-pay-modal-close {
        background: none;
        border: none;
        font-size: 24px;
        cursor: pointer;
        color: #666;
      }
      
      .asian-crypto-pay-modal[data-theme="dark"] .asian-crypto-pay-modal-close {
        color: #ccc;
      }
      
      .asian-crypto-pay-modal-body {
        padding: 16px;
      }
      
      .asian-crypto-pay-modal-footer {
        padding: 16px;
        border-top: 1px solid #eee;
        display: flex;
        justify-content: flex-end;
      }
      
      .asian-crypto-pay-modal[data-theme="dark"] .asian-crypto-pay-modal-footer {
        border-top-color: #444;
      }
      
      /* Payment info styles */
      .asian-crypto-pay-payment-info {
        text-align: center;
      }
      
      .asian-crypto-pay-address {
        background-color: #f5f5f5;
        padding: 8px;
        border-radius: 4px;
        font-family: monospace;
        margin: 8px 0;
        word-break: break-all;
      }
      
      .asian-crypto-pay-modal[data-theme="dark"] .asian-crypto-pay-address {
        background-color: #444;
      }
      
      .asian-crypto-pay-copy-button {
        background-color: #eee;
        border: none;
        border-radius: 4px;
        padding: 4px 8px;
        margin-left: 8px;
        cursor: pointer;
      }
      
      .asian-crypto-pay-modal[data-theme="dark"] .asian-crypto-pay-copy-button {
        background-color: #555;
        color: white;
      }
      
      .asian-crypto-pay-qr-code {
        margin: 16px 0;
      }
      
      .asian-crypto-pay-qr-code img {
        max-width: 200px;
        height: auto;
      }
      
      .asian-crypto-pay-status {
        margin-top: 16px;
      }
      
      .asian-crypto-pay-cancel-button {
        background-color: #f44336;
        color: white;
        border: none;
        border-radius: 4px;
        padding: 8px 16px;
        cursor: pointer;
      }
      
      .asian-crypto-pay-close-button {
        background-color: #2d5afe;
        color: white;
        border: none;
        border-radius: 4px;
        padding: 8px 16px;
        cursor: pointer;
      }
      
      /* Success styles */
      .asian-crypto-pay-success {
        text-align: center;
        padding: 16px;
      }
      
      .asian-crypto-pay-success-icon {
        display: inline-flex;
        justify-content: center;
        align-items: center;
        width: 64px;
        height: 64px;
        background-color: #4caf50;
        color: white;
        border-radius: 50%;
        font-size: 32px;
        margin-bottom: 16px;
      }
      
      /* Form styles */
      .asian-crypto-pay-form {
        max-width: 500px;
        margin: 0 auto;
      }
      
      .asian-crypto-pay-form-group {
        margin-bottom: 16px;
      }
      
      .asian-crypto-pay-form-group label {
        display: block;
        margin-bottom: 8px;
        font-weight: 600;
      }
      
      .asian-crypto-pay-form-group input,
      .asian-crypto-pay-form-group select {
        width: 100%;
        padding: 8px;
        border: 1px solid #ddd;
        border-radius: 4px;
        font-size: 16px;
      }
      
      .asian-crypto-pay-form[data-theme="dark"] {
        color: white;
      }
      
      .asian-crypto-pay-form[data-theme="dark"] input,
      .asian-crypto-pay-form[data-theme="dark"] select {
        background-color: #444;
        border-color: #666;
        color: white;
      }
      
      .asian-crypto-pay-form-actions {
        margin-top: 24px;
      }
      
      .asian-crypto-pay-submit-button {
        display: inline-block;
        padding: 12px 24px;
        background-color: #2d5afe;
        color: white;
        border: none;
        border-radius: 4px;
        font-size: 16px;
        font-weight: 600;
        cursor: pointer;
        transition: background-color 0.2s;
      }
      
      .asian-crypto-pay-submit-button:hover {
        background-color: #0039cb;
      }
      
      .asian-crypto-pay-form[data-theme="dark"] .asian-crypto-pay-submit-button {
        background-color: #5f7aff;
      }
      
      .asian-crypto-pay-form[data-theme="dark"] .asian-crypto-pay-submit-button:hover {
        background-color: #8c9eff;
      }
    `;
    
    // Add style to head
    document.head.appendChild(style);
  }
  
  /**
   * Initialize webhook handling
   * 
   * @param {Object} webhookConfig - Webhook configuration
   * @param {string} webhookConfig.endpoint - Webhook endpoint URL
   * @param {string} webhookConfig.secret - Webhook secret for signature verification
   * @param {Function} webhookConfig.handler - Webhook event handler
   */
  initializeWebhooks(webhookConfig) {
    if (!webhookConfig.endpoint) {
      throw new Error('Webhook endpoint is required');
    }
    
    if (!webhookConfig.secret) {
      throw new Error('Webhook secret is required');
    }
    
    if (!webhookConfig.handler || typeof webhookConfig.handler !== 'function') {
      throw new Error('Webhook handler must be a function');
    }
    
    this.config.webhookConfig = webhookConfig;
    
    console.log('Webhooks initialized');
  }
  
  /**
   * Verify webhook signature
   * 
   * @param {string} signature - Webhook signature
   * @param {string} body - Raw webhook body
   * @returns {boolean} - Whether signature is valid
   */
  verifyWebhookSignature(signature, body) {
    if (!this.config.webhookConfig || !this.config.webhookConfig.secret) {
      throw new Error('Webhooks not initialized');
    }
    
    // In a real implementation, use a proper HMAC library
    const expectedSignature = this._sha256(this.config.webhookConfig.secret + body);
    return signature === expectedSignature;
  }
  
  /**
   * Process webhook event
   * 
   * @param {Object} event - Webhook event
   * @param {string} signature - Webhook signature
   * @returns {boolean} - Whether event was processed successfully
   */
  processWebhookEvent(event, signature) {
    if (!this.config.webhookConfig || !this.config.webhookConfig.handler) {
      throw new Error('Webhooks not initialized');
    }
    
    // Verify signature
    const isValid = this.verifyWebhookSignature(signature, JSON.stringify(event));
    
    if (!isValid) {
      console.error('Invalid webhook signature');
      return false;
    }
    
    // Process event
    try {
      this.config.webhookConfig.handler(event);
      return true;
    } catch (error) {
      console.error('Webhook handler error:', error);
      return false;
    }
  }
}

/**
 * Malaysia Compliance Module
 * Implements regulatory requirements for Malaysia
 */
class MalaysiaComplianceModule {
  constructor(config) {
    this.config = config;
    this.country = 'Malaysia';
    this.currencyCode = 'MYR';
    this.regulator = 'Securities Commission Malaysia (SC)';
    this.requirements = {
      kycThreshold: 3000, // MYR
      reportingThreshold: 25000, // MYR
      supportedCryptocurrencies: ['BTC', 'ETH', 'USDT', 'USDC', 'BNB']
    };
  }
  
  applyRegulations() {
    console.log(`Applying ${this.country} regulations`);
    // Implementation of Malaysia-specific regulations
  }
  
  validatePayment(paymentDetails) {
    // Check if currency is supported
    if (paymentDetails.currency === this.currencyCode) {
      // Apply Malaysia-specific validations
      if (paymentDetails.amount >= this.requirements.kycThreshold && !paymentDetails.customerName) {
        throw new Error(`KYC information required for payments above ${this.requirements.kycThreshold} ${this.currencyCode}`);
      }
    }
    
    return true;
  }
}

/**
 * Singapore Compliance Module
 * Implements regulatory requirements for Singapore
 */
class SingaporeComplianceModule {
  constructor(config) {
    this.config = config;
    this.country = 'Singapore';
    this.currencyCode = 'SGD';
    this.regulator = 'Monetary Authority of Singapore (MAS)';
    this.requirements = {
      kycThreshold: 1000, // SGD
      travelRuleThreshold: 1000, // SGD
      supportedCryptocurrencies: ['BTC', 'ETH', 'USDT', 'USDC', 'BNB']
    };
  }
  
  applyRegulations() {
    console.log(`Applying ${this.country} regulations`);
    // Implementation of Singapore-specific regulations
  }
  
  validatePayment(paymentDetails) {
    // Check if currency is supported
    if (paymentDetails.currency === this.currencyCode) {
      // Apply Singapore-specific validations
      if (paymentDetails.amount >= this.requirements.kycThreshold && 
          (!paymentDetails.customerName || !paymentDetails.customerEmail)) {
        throw new Error(`KYC information required for payments above ${this.requirements.kycThreshold} ${this.currencyCode}`);
      }
      
      // Travel Rule compliance
      if (paymentDetails.amount >= this.requirements.travelRuleThreshold) {
        console.log('Travel Rule applies to this transaction');
        // Additional Travel Rule implementation would go here
      }
    }
    
    return true;
  }
}

/**
 * Indonesia Compliance Module
 * Implements regulatory requirements for Indonesia
 */
class IndonesiaComplianceModule {
  constructor(config) {
    this.config = config;
    this.country = 'Indonesia';
    this.currencyCode = 'IDR';
    this.regulator = 'Commodity Futures Trading Regulatory Agency (Bappebti)';
    this.requirements = {
      kycThreshold: 2000000, // IDR
      supportedCryptocurrencies: ['BTC', 'ETH', 'USDT', 'USDC', 'BNB']
    };
  }
  
  applyRegulations() {
    console.log(`Applying ${this.country} regulations`);
    // Implementation of Indonesia-specific regulations
  }
  
  validatePayment(paymentDetails) {
    // Check if currency is supported
    if (paymentDetails.currency === this.currencyCode) {
      // Apply Indonesia-specific validations
      if (paymentDetails.amount >= this.requirements.kycThreshold && !paymentDetails.customerName) {
        throw new Error(`KYC information required for payments above ${this.requirements.kycThreshold} ${this.currencyCode}`);
      }
    }
    
    return true;
  }
}

/**
 * Thailand Compliance Module
 * Implements regulatory requirements for Thailand
 */
class ThailandComplianceModule {
  constructor(config) {
    this.config = config;
    this.country = 'Thailand';
    this.currencyCode = 'THB';
    this.regulator = 'Securities and Exchange Commission (SEC)';
    this.requirements = {
      kycThreshold: 50000, // THB
      supportedCryptocurrencies: ['BTC', 'ETH', 'USDT', 'USDC', 'BNB']
    };
  }
  
  applyRegulations() {
    console.log(`Applying ${this.country} regulations`);
    // Implementation of Thailand-specific regulations
  }
  
  validatePayment(paymentDetails) {
    // Check if currency is supported
    if (paymentDetails.currency === this.currencyCode) {
      // Apply Thailand-specific validations
      if (paymentDetails.amount >= this.requirements.kycThreshold && !paymentDetails.customerName) {
        throw new Error(`KYC information required for payments above ${this.requirements.kycThreshold} ${this.currencyCode}`);
      }
    }
    
    return true;
  }
}

/**
 * Brunei Compliance Module
 * Implements regulatory requirements for Brunei
 */
class BruneiComplianceModule {
  constructor(config) {
    this.config = config;
    this.country = 'Brunei';
    this.currencyCode = 'BND';
    this.regulator = 'Autoriti Monetari Brunei Darussalam (AMBD)';
    this.requirements = {
      kycThreshold: 5000, // BND
      supportedCryptocurrencies: ['BTC', 'ETH', 'USDT', 'USDC', 'BNB']
    };
  }
  
  applyRegulations() {
    console.log(`Applying ${this.country} regulations`);
    // Implementation of Brunei-specific regulations
  }
  
  validatePayment(paymentDetails) {
    // Check if currency is supported
    if (paymentDetails.currency === this.currencyCode) {
      // Apply Brunei-specific validations
      if (paymentDetails.amount >= this.requirements.kycThreshold && !paymentDetails.customerName) {
        throw new Error(`KYC information required for payments above ${this.requirements.kycThreshold} ${this.currencyCode}`);
      }
    }
    
    return true;
  }
}

/**
 * Cambodia Compliance Module
 * Implements regulatory requirements for Cambodia
 */
class CambodiaComplianceModule {
  constructor(config) {
    this.config = config;
    this.country = 'Cambodia';
    this.currencyCode = 'KHR';
    this.regulator = 'National Bank of Cambodia (NBC)';
    this.requirements = {
      kycThreshold: 4000000, // KHR
      supportedCryptocurrencies: ['BTC', 'ETH', 'USDT', 'USDC', 'BNB']
    };
  }
  
  applyRegulations() {
    console.log(`Applying ${this.country} regulations`);
    // Implementation of Cambodia-specific regulations
  }
  
  validatePayment(paymentDetails) {
    // Check if currency is supported
    if (paymentDetails.currency === this.currencyCode) {
      // Apply Cambodia-specific validations
      if (paymentDetails.amount >= this.requirements.kycThreshold && !paymentDetails.customerName) {
        throw new Error(`KYC information required for payments above ${this.requirements.kycThreshold} ${this.currencyCode}`);
      }
    }
    
    return true;
  }
}

/**
 * Vietnam Compliance Module
 * Implements regulatory requirements for Vietnam
 */
class VietnamComplianceModule {
  constructor(config) {
    this.config = config;
    this.country = 'Vietnam';
    this.currencyCode = 'VND';
    this.regulator = 'State Bank of Vietnam (SBV)';
    this.requirements = {
      kycThreshold: 20000000, // VND
      supportedCryptocurrencies: ['BTC', 'ETH', 'USDT', 'USDC', 'BNB']
    };
  }
  
  applyRegulations() {
    console.log(`Applying ${this.country} regulations`);
    // Implementation of Vietnam-specific regulations
  }
  
  validatePayment(paymentDetails) {
    // Check if currency is supported
    if (paymentDetails.currency === this.currencyCode) {
      // Apply Vietnam-specific validations
      if (paymentDetails.amount >= this.requirements.kycThreshold && !paymentDetails.customerName) {
        throw new Error(`KYC information required for payments above ${this.requirements.kycThreshold} ${this.currencyCode}`);
      }
    }
    
    return true;
  }
}

/**
 * Laos Compliance Module
 * Implements regulatory requirements for Laos
 */
class LaosComplianceModule {
  constructor(config) {
    this.config = config;
    this.country = 'Laos';
    this.currencyCode = 'LAK';
    this.regulator = 'Bank of the Lao PDR (BOL)';
    this.requirements = {
      kycThreshold: 9000000, // LAK
      supportedCryptocurrencies: ['BTC', 'ETH', 'USDT', 'USDC', 'BNB']
    };
  }
  
  applyRegulations() {
    console.log(`Applying ${this.country} regulations`);
    // Implementation of Laos-specific regulations
  }
  
  validatePayment(paymentDetails) {
    // Check if currency is supported
    if (paymentDetails.currency === this.currencyCode) {
      // Apply Laos-specific validations
      if (paymentDetails.amount >= this.requirements.kycThreshold && !paymentDetails.customerName) {
        throw new Error(`KYC information required for payments above ${this.requirements.kycThreshold} ${this.currencyCode}`);
      }
    }
    
    return true;
  }
}

// Export the SDK
if (typeof module !== 'undefined' && typeof module.exports !== 'undefined') {
  module.exports = AsianCryptoPayment;
} else {
  window.AsianCryptoPayment = AsianCryptoPayment;
}
