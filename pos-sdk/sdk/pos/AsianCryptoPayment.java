/**
 * Asian Cryptocurrency Payment System - POS Terminal SDK
 * Version: 1.0.0
 * 
 * This SDK provides a comprehensive interface for integrating cryptocurrency
 * payments into Point-of-Sale terminals across Asian markets, with support for 
 * regulatory compliance in Malaysia, Singapore, Indonesia, Thailand, Brunei, 
 * Cambodia, Vietnam, and Laos.
 */

package com.asiancryptopay.sdk;

import android.content.Context;
import android.graphics.Bitmap;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.math.BigDecimal;
import java.security.InvalidKeyException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.TimeZone;
import java.util.UUID;
import java.util.concurrent.TimeUnit;

import javax.crypto.Mac;
import javax.crypto.spec.SecretKeySpec;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.MediaType;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;

/**
 * Main SDK class for Asian Cryptocurrency Payment System
 */
public class AsianCryptoPayment {
    private static final String TAG = "AsianCryptoPayment";
    private static final String SDK_VERSION = "1.0.0";
    private static final MediaType JSON = MediaType.parse("application/json; charset=utf-8");
    private static final String DEFAULT_API_ENDPOINT = "https://api.asiancryptopay.com";
    
    // Configuration
    private final String apiKey;
    private final String merchantId;
    private final String countryCode;
    private final boolean testMode;
    private final String apiEndpoint;
    private final List<String> supportedCryptocurrencies;
    private final Map<String, Object> webhookConfig;
    
    // HTTP Client
    private final OkHttpClient httpClient;
    
    // Country-specific compliance module
    private final CountryComplianceModule countryModule;
    
    // Security module
    private final SecurityModule securityModule;
    
    // Android context
    private final Context context;
    
    // Handler for UI thread callbacks
    private final Handler mainHandler;
    
    /**
     * Builder class for AsianCryptoPayment
     */
    public static class Builder {
        private final String apiKey;
        private final String merchantId;
        private final String countryCode;
        private final Context context;
        
        private boolean testMode = false;
        private String apiEndpoint = DEFAULT_API_ENDPOINT;
        private List<String> supportedCryptocurrencies = new ArrayList<>();
        private Map<String, Object> webhookConfig = null;
        
        /**
         * Initialize builder with required parameters
         * 
         * @param context Android context
         * @param apiKey Merchant API key
         * @param merchantId Merchant ID
         * @param countryCode Two-letter country code (MY, SG, ID, TH, BN, KH, VN, LA)
         */
        public Builder(@NonNull Context context, @NonNull String apiKey, @NonNull String merchantId, @NonNull String countryCode) {
            this.context = context.getApplicationContext();
            this.apiKey = apiKey;
            this.merchantId = merchantId;
            this.countryCode = countryCode;
            
            // Default supported cryptocurrencies
            this.supportedCryptocurrencies.add("BTC");
            this.supportedCryptocurrencies.add("ETH");
            this.supportedCryptocurrencies.add("USDT");
            this.supportedCryptocurrencies.add("USDC");
            this.supportedCryptocurrencies.add("BNB");
        }
        
        /**
         * Set test mode
         * 
         * @param testMode Whether to use test mode
         * @return Builder instance
         */
        public Builder setTestMode(boolean testMode) {
            this.testMode = testMode;
            return this;
        }
        
        /**
         * Set custom API endpoint
         * 
         * @param apiEndpoint Custom API endpoint
         * @return Builder instance
         */
        public Builder setApiEndpoint(@NonNull String apiEndpoint) {
            this.apiEndpoint = apiEndpoint;
            return this;
        }
        
        /**
         * Set supported cryptocurrencies
         * 
         * @param supportedCryptocurrencies List of supported cryptocurrencies
         * @return Builder instance
         */
        public Builder setSupportedCryptocurrencies(@NonNull List<String> supportedCryptocurrencies) {
            this.supportedCryptocurrencies = new ArrayList<>(supportedCryptocurrencies);
            return this;
        }
        
        /**
         * Set webhook configuration
         * 
         * @param webhookEndpoint Webhook endpoint URL
         * @param webhookSecret Webhook secret for signature verification
         * @return Builder instance
         */
        public Builder setWebhookConfig(@NonNull String webhookEndpoint, @NonNull String webhookSecret) {
            this.webhookConfig = new HashMap<>();
            this.webhookConfig.put("endpoint", webhookEndpoint);
            this.webhookConfig.put("secret", webhookSecret);
            return this;
        }
        
        /**
         * Build AsianCryptoPayment instance
         * 
         * @return AsianCryptoPayment instance
         */
        public AsianCryptoPayment build() {
            return new AsianCryptoPayment(this);
        }
    }
    
    /**
     * Private constructor, use Builder instead
     * 
     * @param builder Builder instance
     */
    private AsianCryptoPayment(Builder builder) {
        this.context = builder.context;
        this.apiKey = builder.apiKey;
        this.merchantId = builder.merchantId;
        this.countryCode = builder.countryCode;
        this.testMode = builder.testMode;
        this.apiEndpoint = builder.apiEndpoint;
        this.supportedCryptocurrencies = builder.supportedCryptocurrencies;
        this.webhookConfig = builder.webhookConfig;
        
        // Initialize HTTP client
        this.httpClient = new OkHttpClient.Builder()
                .connectTimeout(30, TimeUnit.SECONDS)
                .readTimeout(30, TimeUnit.SECONDS)
                .writeTimeout(30, TimeUnit.SECONDS)
                .build();
        
        // Initialize main thread handler
        this.mainHandler = new Handler(Looper.getMainLooper());
        
        // Initialize security module
        this.securityModule = new SecurityModule(this.apiKey);
        
        // Initialize country-specific module
        this.countryModule = createCountryModule(this.countryCode);
        
        Log.i(TAG, "SDK initialized for country: " + this.countryCode);
    }
    
    /**
     * Create country-specific compliance module
     * 
     * @param countryCode Two-letter country code
     * @return CountryComplianceModule instance
     */
    private CountryComplianceModule createCountryModule(String countryCode) {
        switch (countryCode) {
            case "MY":
                return new MalaysiaComplianceModule();
            case "SG":
                return new SingaporeComplianceModule();
            case "ID":
                return new IndonesiaComplianceModule();
            case "TH":
                return new ThailandComplianceModule();
            case "BN":
                return new BruneiComplianceModule();
            case "KH":
                return new CambodiaComplianceModule();
            case "VN":
                return new VietnamComplianceModule();
            case "LA":
                return new LaosComplianceModule();
            default:
                throw new IllegalArgumentException("Unsupported country code: " + countryCode);
        }
    }
    
    /**
     * Create a new cryptocurrency payment
     * 
     * @param paymentDetails Payment details
     * @param callback Callback for payment result
     */
    public void createPayment(@NonNull PaymentDetails paymentDetails, @NonNull PaymentCallback callback) {
        // Validate payment details
        try {
            validatePaymentDetails(paymentDetails);
        } catch (IllegalArgumentException e) {
            callback.onError(e);
            return;
        }
        
        // Apply country-specific validations
        try {
            countryModule.validatePayment(paymentDetails);
        } catch (IllegalArgumentException e) {
            callback.onError(e);
            return;
        }
        
        // Prepare payment data
        JSONObject paymentData = new JSONObject();
        try {
            paymentData.put("merchant_id", merchantId);
            paymentData.put("amount", paymentDetails.getAmount().toString());
            paymentData.put("currency", paymentDetails.getCurrency());
            paymentData.put("crypto_currency", paymentDetails.getCryptoCurrency());
            paymentData.put("description", paymentDetails.getDescription());
            paymentData.put("order_id", paymentDetails.getOrderId() != null ? paymentDetails.getOrderId() : "order-" + System.currentTimeMillis());
            paymentData.put("customer_email", paymentDetails.getCustomerEmail());
            paymentData.put("customer_name", paymentDetails.getCustomerName());
            paymentData.put("callback_url", paymentDetails.getCallbackUrl());
            paymentData.put("success_url", paymentDetails.getSuccessUrl());
            paymentData.put("cancel_url", paymentDetails.getCancelUrl());
            paymentData.put("country_code", countryCode);
            paymentData.put("test_mode", testMode);
            
            // Add metadata if available
            if (paymentDetails.getMetadata() != null) {
                paymentData.put("metadata", new JSONObject(paymentDetails.getMetadata()));
            }
        } catch (JSONException e) {
            callback.onError(new RuntimeException("Failed to create payment data", e));
            return;
        }
        
        // Make API request
        String endpoint = "payments";
        makeApiRequest(endpoint, "POST", paymentData, new ApiCallback() {
            @Override
            public void onSuccess(JSONObject response) {
                try {
                    Payment payment = Payment.fromJson(response);
                    callback.onSuccess(payment);
                } catch (JSONException e) {
                    callback.onError(new RuntimeException("Failed to parse payment response", e));
                }
            }
            
            @Override
            public void onError(Exception e) {
                callback.onError(e);
            }
        });
    }
    
    /**
     * Get payment details by ID
     * 
     * @param paymentId Payment ID
     * @param callback Callback for payment result
     */
    public void getPayment(@NonNull String paymentId, @NonNull PaymentCallback callback) {
        if (paymentId.isEmpty()) {
            callback.onError(new IllegalArgumentException("Payment ID is required"));
            return;
        }
        
        String endpoint = "payments/" + paymentId;
        makeApiRequest(endpoint, "GET", null, new ApiCallback() {
            @Override
            public void onSuccess(JSONObject response) {
                try {
                    Payment payment = Payment.fromJson(response);
                    callback.onSuccess(payment);
                } catch (JSONException e) {
                    callback.onError(new RuntimeException("Failed to parse payment response", e));
                }
            }
            
            @Override
            public void onError(Exception e) {
                callback.onError(e);
            }
        });
    }
    
    /**
     * Get list of payments
     * 
     * @param filters Filter parameters
     * @param callback Callback for payments result
     */
    public void getPayments(@Nullable PaymentFilters filters, @NonNull PaymentsListCallback callback) {
        StringBuilder endpoint = new StringBuilder("payments");
        
        // Add query parameters if filters are provided
        if (filters != null) {
            endpoint.append("?");
            
            if (filters.getStatus() != null) {
                endpoint.append("status=").append(filters.getStatus()).append("&");
            }
            
            if (filters.getFromDate() != null) {
                SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd", Locale.US);
                sdf.setTimeZone(TimeZone.getTimeZone("UTC"));
                endpoint.append("from_date=").append(sdf.format(filters.getFromDate())).append("&");
            }
            
            if (filters.getToDate() != null) {
                SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd", Locale.US);
                sdf.setTimeZone(TimeZone.getTimeZone("UTC"));
                endpoint.append("to_date=").append(sdf.format(filters.getToDate())).append("&");
            }
            
            if (filters.getLimit() > 0) {
                endpoint.append("limit=").append(filters.getLimit()).append("&");
            }
            
            if (filters.getOffset() > 0) {
                endpoint.append("offset=").append(filters.getOffset()).append("&");
            }
            
            // Remove trailing '&' if present
            if (endpoint.charAt(endpoint.length() - 1) == '&') {
                endpoint.deleteCharAt(endpoint.length() - 1);
            }
        }
        
        makeApiRequest(endpoint.toString(), "GET", null, new ApiCallback() {
            @Override
            public void onSuccess(JSONObject response) {
                try {
                    List<Payment> payments = new ArrayList<>();
                    JSONArray paymentsArray = response.getJSONArray("payments");
                    
                    for (int i = 0; i < paymentsArray.length(); i++) {
                        JSONObject paymentJson = paymentsArray.getJSONObject(i);
                        payments.add(Payment.fromJson(paymentJson));
                    }
                    
                    int total = response.getInt("total");
                    callback.onSuccess(payments, total);
                } catch (JSONException e) {
                    callback.onError(new RuntimeException("Failed to parse payments response", e));
                }
            }
            
            @Override
            public void onError(Exception e) {
                callback.onError(e);
            }
        });
    }
    
    /**
     * Cancel a payment
     * 
     * @param paymentId Payment ID
     * @param callback Callback for payment result
     */
    public void cancelPayment(@NonNull String paymentId, @NonNull PaymentCallback callback) {
        if (paymentId.isEmpty()) {
            callback.onError(new IllegalArgumentException("Payment ID is required"));
            return;
        }
        
        String endpoint = "payments/" + paymentId + "/cancel";
        makeApiRequest(endpoint, "POST", null, new ApiCallback() {
            @Override
            public void onSuccess(JSONObject response) {
                try {
                    Payment payment = Payment.fromJson(response);
                    callback.onSuccess(payment);
                } catch (JSONException e) {
                    callback.onError(new RuntimeException("Failed to parse payment response", e));
                }
            }
            
            @Override
            public void onError(Exception e) {
                callback.onError(e);
            }
        });
    }
    
    /**
     * Get current exchange rates
     * 
     * @param baseCurrency Base currency
     * @param cryptoCurrencies List of cryptocurrencies to get rates for
     * @param callback Callback for exchange rates result
     */
    public void getExchangeRates(@NonNull String baseCurrency, @Nullable List<String> cryptoCurrencies, @NonNull ExchangeRatesCallback callback) {
        if (baseCurrency.isEmpty()) {
            callback.onError(new IllegalArgumentException("Base currency is required"));
            return;
        }
        
        List<String> currencies = cryptoCurrencies != null ? cryptoCurrencies : supportedCryptocurrencies;
        
        StringBuilder endpoint = new StringBuilder("exchange-rates?base_currency=");
        endpoint.append(baseCurrency);
        endpoint.append("&currencies=");
        
        for (int i = 0; i < currencies.size(); i++) {
            endpoint.append(currencies.get(i));
            if (i < currencies.size() - 1) {
                endpoint.append(",");
            }
        }
        
        makeApiRequest(endpoint.toString(), "GET", null, new ApiCallback() {
            @Override
            public void onSuccess(JSONObject response) {
                try {
                    Map<String, BigDecimal> rates = new HashMap<>();
                    JSONObject ratesJson = response.getJSONObject("rates");
                    
                    for (String currency : currencies) {
                        if (ratesJson.has(currency)) {
                            rates.put(currency, new BigDecimal(ratesJson.getString(currency)));
                        }
                    }
                    
                    callback.onSuccess(baseCurrency, rates);
                } catch (JSONException e) {
                    callback.onError(new RuntimeException("Failed to parse exchange rates response", e));
                }
            }
            
            @Override
            public void onError(Exception e) {
                callback.onError(e);
            }
        });
    }
    
    /**
     * Verify webhook signature
     * 
     * @param signature Webhook signature
     * @param body Raw webhook body
     * @return Whether signature is valid
     */
    public boolean verifyWebhookSignature(@NonNull String signature, @NonNull String body) {
        if (webhookConfig == null || !webhookConfig.containsKey("secret")) {
            throw new IllegalStateException("Webhooks not initialized");
        }
        
        String secret = (String) webhookConfig.get("secret");
        return securityModule.verifySignature(signature, body, secret);
    }
    
    /**
     * Process webhook event
     * 
     * @param event Webhook event
     * @param signature Webhook signature
     * @param webhookHandler Webhook handler
     * @return Whether event was processed successfully
     */
    public boolean processWebhookEvent(@NonNull JSONObject event, @NonNull String signature, @NonNull WebhookHandler webhookHandler) {
        if (webhookConfig == null) {
            throw new IllegalStateException("Webhooks not initialized");
        }
        
        // Verify signature
        boolean isValid = verifyWebhookSignature(signature, event.toString());
        
        if (!isValid) {
            Log.e(TAG, "Invalid webhook signature");
            return false;
        }
        
        // Process event
        try {
            String eventType = event.getString("type");
            
            switch (eventType) {
                case "payment.created":
                    webhookHandler.onPaymentCreated(Payment.fromJson(event.getJSONObject("data")));
                    break;
                case "payment.updated":
                    webhookHandler.onPaymentUpdated(Payment.fromJson(event.getJSONObject("data")));
                    break;
                case "payment.completed":
                    webhookHandler.onPaymentCompleted(Payment.fromJson(event.getJSONObject("data")));
                    break;
                case "payment.cancelled":
                    webhookHandler.onPaymentCancelled(Payment.fromJson(event.getJSONObject("data")));
                    break;
                case "payment.expired":
                    webhookHandler.onPaymentExpired(Payment.fromJson(event.getJSONObject("data")));
                    break;
                default:
                    webhookHandler.onUnknownEvent(eventType, event);
                    break;
            }
            
            return true;
        } catch (JSONException e) {
            Log.e(TAG, "Failed to process webhook event", e);
            return false;
        }
    }
    
    /**
     * Validate payment details
     * 
     * @param paymentDetails Payment details
     * @throws IllegalArgumentException if validation fails
     */
    private void validatePaymentDetails(PaymentDetails paymentDetails) {
        if (paymentDetails.getAmount() == null || paymentDetails.getAmount().compareTo(BigDecimal.ZERO) <= 0) {
            throw new IllegalArgumentException("Payment amount must be greater than zero");
        }
        
        if (paymentDetails.getCurrency() == null || paymentDetails.getCurrency().isEmpty()) {
            throw new IllegalArgumentException("Currency is required");
        }
        
        if (paymentDetails.getCryptoCurrency() == null || paymentDetails.getCryptoCurrency().isEmpty()) {
            throw new IllegalArgumentException("Cryptocurrency is required");
        }
        
        if (!supportedCryptocurrencies.contains(paymentDetails.getCryptoCurrency())) {
            throw new IllegalArgumentException("Unsupported cryptocurrency. Must be one of: " + supportedCryptocurrencies);
        }
    }
    
    /**
     * Make API request
     * 
     * @param endpoint API endpoint
     * @param method HTTP method
     * @param data Request payload
     * @param callback Callback for API response
     */
    private void makeApiRequest(String endpoint, String method, @Nullable JSONObject data, ApiCallback callback) {
        String url = apiEndpoint + "/" + endpoint;
        String timestamp = String.valueOf(System.currentTimeMillis());
        
        Request.Builder requestBuilder = new Request.Builder()
                .url(url)
                .header("X-Merchant-ID", merchantId)
                .header("X-Timestamp", timestamp)
                .header("X-Test-Mode", testMode ? "true" : "false")
                .header("User-Agent", "AsianCryptoPayment-Android/" + SDK_VERSION);
        
        if (data != null) {
            String signature = securityModule.generateSignature(data.toString(), timestamp);
            requestBuilder.header("X-Signature", signature);
            
            RequestBody requestBody = RequestBody.create(data.toString(), JSON);
            requestBuilder.method(method, requestBody);
        } else if (method.equals("POST")) {
            RequestBody requestBody = RequestBody.create("", JSON);
            requestBuilder.method(method, requestBody);
        } else {
            requestBuilder.method(method, null);
        }
        
        httpClient.newCall(requestBuilder.build()).enqueue(new Callback() {
            @Override
            public void onFailure(@NonNull Call call, @NonNull IOException e) {
                mainHandler.post(() -> callback.onError(e));
            }
            
            @Override
            public void onResponse(@NonNull Call call, @NonNull Response response) {
                try {
                    String responseBody = response.body() != null ? response.body().string() : "{}";
                    final JSONObject jsonResponse = new JSONObject(responseBody);
                    
                    if (!response.isSuccessful()) {
                        String errorMessage = jsonResponse.optString("message", "API request failed");
                        mainHandler.post(() -> callback.onError(new RuntimeException(errorMessage)));
                        return;
                    }
                    
                    mainHandler.post(() -> callback.onSuccess(jsonResponse));
                } catch (IOException | JSONException e) {
                    mainHandler.post(() -> callback.onError(e));
                }
            }
        });
    }
    
    /**
     * Payment details class
     */
    public static class PaymentDetails {
        private BigDecimal amount;
        private String currency;
        private String cryptoCurrency;
        private String description;
        private String orderId;
        private String customerEmail;
        private String customerName;
        private String callbackUrl;
        private String successUrl;
        private String cancelUrl;
        private Map<String, Object> metadata;
        
        private PaymentDetails(Builder builder) {
            this.amount = builder.amount;
            this.currency = builder.currency;
            this.cryptoCurrency = builder.cryptoCurrency;
            this.description = builder.description;
            this.orderId = builder.orderId;
            this.customerEmail = builder.customerEmail;
            this.customerName = builder.customerName;
            this.callbackUrl = builder.callbackUrl;
            this.successUrl = builder.successUrl;
            this.cancelUrl = builder.cancelUrl;
            this.metadata = builder.metadata;
        }
        
        public BigDecimal getAmount() {
            return amount;
        }
        
        public String getCurrency() {
            return currency;
        }
        
        public String getCryptoCurrency() {
            return cryptoCurrency;
        }
        
        public String getDescription() {
            return description;
        }
        
        public String getOrderId() {
            return orderId;
        }
        
        public String getCustomerEmail() {
            return customerEmail;
        }
        
        public String getCustomerName() {
            return customerName;
        }
        
        public String getCallbackUrl() {
            return callbackUrl;
        }
        
        public String getSuccessUrl() {
            return successUrl;
        }
        
        public String getCancelUrl() {
            return cancelUrl;
        }
        
        public Map<String, Object> getMetadata() {
            return metadata;
        }
        
        /**
         * Builder for PaymentDetails
         */
        public static class Builder {
            private final BigDecimal amount;
            private final String currency;
            private final String cryptoCurrency;
            
            private String description = "";
            private String orderId = null;
            private String customerEmail = "";
            private String customerName = "";
            private String callbackUrl = "";
            private String successUrl = "";
            private String cancelUrl = "";
            private Map<String, Object> metadata = null;
            
            /**
             * Initialize builder with required parameters
             * 
             * @param amount Payment amount
             * @param currency Fiat currency code (e.g., MYR, SGD)
             * @param cryptoCurrency Cryptocurrency code (e.g., BTC, ETH)
             */
            public Builder(@NonNull BigDecimal amount, @NonNull String currency, @NonNull String cryptoCurrency) {
                this.amount = amount;
                this.currency = currency;
                this.cryptoCurrency = cryptoCurrency;
            }
            
            /**
             * Set payment description
             * 
             * @param description Payment description
             * @return Builder instance
             */
            public Builder setDescription(@NonNull String description) {
                this.description = description;
                return this;
            }
            
            /**
             * Set merchant order ID
             * 
             * @param orderId Merchant order ID
             * @return Builder instance
             */
            public Builder setOrderId(@NonNull String orderId) {
                this.orderId = orderId;
                return this;
            }
            
            /**
             * Set customer email
             * 
             * @param customerEmail Customer email
             * @return Builder instance
             */
            public Builder setCustomerEmail(@NonNull String customerEmail) {
                this.customerEmail = customerEmail;
                return this;
            }
            
            /**
             * Set customer name
             * 
             * @param customerName Customer name
             * @return Builder instance
             */
            public Builder setCustomerName(@NonNull String customerName) {
                this.customerName = customerName;
                return this;
            }
            
            /**
             * Set callback URL for payment updates
             * 
             * @param callbackUrl Callback URL
             * @return Builder instance
             */
            public Builder setCallbackUrl(@NonNull String callbackUrl) {
                this.callbackUrl = callbackUrl;
                return this;
            }
            
            /**
             * Set redirect URL on successful payment
             * 
             * @param successUrl Success URL
             * @return Builder instance
             */
            public Builder setSuccessUrl(@NonNull String successUrl) {
                this.successUrl = successUrl;
                return this;
            }
            
            /**
             * Set redirect URL on cancelled payment
             * 
             * @param cancelUrl Cancel URL
             * @return Builder instance
             */
            public Builder setCancelUrl(@NonNull String cancelUrl) {
                this.cancelUrl = cancelUrl;
                return this;
            }
            
            /**
             * Set additional metadata
             * 
             * @param metadata Additional metadata
             * @return Builder instance
             */
            public Builder setMetadata(@NonNull Map<String, Object> metadata) {
                this.metadata = new HashMap<>(metadata);
                return this;
            }
            
            /**
             * Build PaymentDetails instance
             * 
             * @return PaymentDetails instance
             */
            public PaymentDetails build() {
                return new PaymentDetails(this);
            }
        }
    }
    
    /**
     * Payment class
     */
    public static class Payment {
        private String id;
        private String merchantId;
        private BigDecimal amount;
        private String currency;
        private BigDecimal cryptoAmount;
        private String cryptoCurrency;
        private String description;
        private String orderId;
        private String customerEmail;
        private String customerName;
        private String address;
        private String qrCodeUrl;
        private String status;
        private Date createdAt;
        private Date updatedAt;
        private Date expiresAt;
        private Map<String, Object> metadata;
        
        private Payment() {
            // Private constructor, use fromJson instead
        }
        
        /**
         * Create Payment instance from JSON
         * 
         * @param json JSON object
         * @return Payment instance
         * @throws JSONException if JSON parsing fails
         */
        public static Payment fromJson(JSONObject json) throws JSONException {
            Payment payment = new Payment();
            
            payment.id = json.getString("id");
            payment.merchantId = json.getString("merchant_id");
            payment.amount = new BigDecimal(json.getString("amount"));
            payment.currency = json.getString("currency");
            payment.cryptoAmount = new BigDecimal(json.getString("crypto_amount"));
            payment.cryptoCurrency = json.getString("crypto_currency");
            payment.description = json.getString("description");
            payment.orderId = json.getString("order_id");
            payment.customerEmail = json.optString("customer_email", "");
            payment.customerName = json.optString("customer_name", "");
            payment.address = json.getString("address");
            payment.qrCodeUrl = json.getString("qr_code_url");
            payment.status = json.getString("status");
            
            SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss'Z'", Locale.US);
            sdf.setTimeZone(TimeZone.getTimeZone("UTC"));
            
            try {
                payment.createdAt = sdf.parse(json.getString("created_at"));
                payment.updatedAt = sdf.parse(json.getString("updated_at"));
                payment.expiresAt = sdf.parse(json.getString("expires_at"));
            } catch (Exception e) {
                throw new JSONException("Failed to parse dates: " + e.getMessage());
            }
            
            if (json.has("metadata") && !json.isNull("metadata")) {
                payment.metadata = new HashMap<>();
                JSONObject metadataJson = json.getJSONObject("metadata");
                
                for (String key : metadataJson.keySet()) {
                    payment.metadata.put(key, metadataJson.get(key));
                }
            }
            
            return payment;
        }
        
        public String getId() {
            return id;
        }
        
        public String getMerchantId() {
            return merchantId;
        }
        
        public BigDecimal getAmount() {
            return amount;
        }
        
        public String getCurrency() {
            return currency;
        }
        
        public BigDecimal getCryptoAmount() {
            return cryptoAmount;
        }
        
        public String getCryptoCurrency() {
            return cryptoCurrency;
        }
        
        public String getDescription() {
            return description;
        }
        
        public String getOrderId() {
            return orderId;
        }
        
        public String getCustomerEmail() {
            return customerEmail;
        }
        
        public String getCustomerName() {
            return customerName;
        }
        
        public String getAddress() {
            return address;
        }
        
        public String getQrCodeUrl() {
            return qrCodeUrl;
        }
        
        public String getStatus() {
            return status;
        }
        
        public Date getCreatedAt() {
            return createdAt;
        }
        
        public Date getUpdatedAt() {
            return updatedAt;
        }
        
        public Date getExpiresAt() {
            return expiresAt;
        }
        
        public Map<String, Object> getMetadata() {
            return metadata;
        }
        
        /**
         * Check if payment is completed
         * 
         * @return Whether payment is completed
         */
        public boolean isCompleted() {
            return "completed".equals(status);
        }
        
        /**
         * Check if payment is pending
         * 
         * @return Whether payment is pending
         */
        public boolean isPending() {
            return "pending".equals(status);
        }
        
        /**
         * Check if payment is expired
         * 
         * @return Whether payment is expired
         */
        public boolean isExpired() {
            return "expired".equals(status);
        }
        
        /**
         * Check if payment is cancelled
         * 
         * @return Whether payment is cancelled
         */
        public boolean isCancelled() {
            return "cancelled".equals(status);
        }
    }
    
    /**
     * Payment filters class
     */
    public static class PaymentFilters {
        private String status;
        private Date fromDate;
        private Date toDate;
        private int limit;
        private int offset;
        
        private PaymentFilters(Builder builder) {
            this.status = builder.status;
            this.fromDate = builder.fromDate;
            this.toDate = builder.toDate;
            this.limit = builder.limit;
            this.offset = builder.offset;
        }
        
        public String getStatus() {
            return status;
        }
        
        public Date getFromDate() {
            return fromDate;
        }
        
        public Date getToDate() {
            return toDate;
        }
        
        public int getLimit() {
            return limit;
        }
        
        public int getOffset() {
            return offset;
        }
        
        /**
         * Builder for PaymentFilters
         */
        public static class Builder {
            private String status;
            private Date fromDate;
            private Date toDate;
            private int limit = 20;
            private int offset = 0;
            
            /**
             * Set status filter
             * 
             * @param status Payment status
             * @return Builder instance
             */
            public Builder setStatus(@NonNull String status) {
                this.status = status;
                return this;
            }
            
            /**
             * Set date range start
             * 
             * @param fromDate Date range start
             * @return Builder instance
             */
            public Builder setFromDate(@NonNull Date fromDate) {
                this.fromDate = fromDate;
                return this;
            }
            
            /**
             * Set date range end
             * 
             * @param toDate Date range end
             * @return Builder instance
             */
            public Builder setToDate(@NonNull Date toDate) {
                this.toDate = toDate;
                return this;
            }
            
            /**
             * Set result limit
             * 
             * @param limit Number of results to return
             * @return Builder instance
             */
            public Builder setLimit(int limit) {
                this.limit = limit;
                return this;
            }
            
            /**
             * Set result offset
             * 
             * @param offset Offset for pagination
             * @return Builder instance
             */
            public Builder setOffset(int offset) {
                this.offset = offset;
                return this;
            }
            
            /**
             * Build PaymentFilters instance
             * 
             * @return PaymentFilters instance
             */
            public PaymentFilters build() {
                return new PaymentFilters(this);
            }
        }
    }
    
    /**
     * Security module for cryptographic operations
     */
    private static class SecurityModule {
        private final String apiKey;
        
        /**
         * Initialize security module
         * 
         * @param apiKey API key for signature generation
         */
        public SecurityModule(String apiKey) {
            this.apiKey = apiKey;
        }
        
        /**
         * Generate signature for API request
         * 
         * @param payload Request payload
         * @param timestamp Request timestamp
         * @return Signature
         */
        public String generateSignature(String payload, String timestamp) {
            String message = timestamp + "." + payload;
            return hmacSha256(apiKey, message);
        }
        
        /**
         * Verify webhook signature
         * 
         * @param signature Webhook signature
         * @param payload Webhook payload
         * @param secret Webhook secret
         * @return Whether signature is valid
         */
        public boolean verifySignature(String signature, String payload, String secret) {
            String expectedSignature = hmacSha256(secret, payload);
            return expectedSignature.equals(signature);
        }
        
        /**
         * Generate HMAC-SHA256 signature
         * 
         * @param key Secret key
         * @param message Message to sign
         * @return HMAC-SHA256 signature
         */
        private String hmacSha256(String key, String message) {
            try {
                Mac sha256Hmac = Mac.getInstance("HmacSHA256");
                SecretKeySpec secretKey = new SecretKeySpec(key.getBytes(), "HmacSHA256");
                sha256Hmac.init(secretKey);
                
                byte[] hash = sha256Hmac.doFinal(message.getBytes());
                return bytesToHex(hash);
            } catch (NoSuchAlgorithmException | InvalidKeyException e) {
                Log.e(TAG, "Failed to generate HMAC-SHA256 signature", e);
                return "";
            }
        }
        
        /**
         * Convert bytes to hexadecimal string
         * 
         * @param bytes Byte array
         * @return Hexadecimal string
         */
        private String bytesToHex(byte[] bytes) {
            StringBuilder hexString = new StringBuilder();
            
            for (byte b : bytes) {
                String hex = Integer.toHexString(0xff & b);
                if (hex.length() == 1) {
                    hexString.append('0');
                }
                hexString.append(hex);
            }
            
            return hexString.toString();
        }
    }
    
    /**
     * Interface for country-specific compliance modules
     */
    private interface CountryComplianceModule {
        /**
         * Validate payment according to country-specific regulations
         * 
         * @param paymentDetails Payment details
         * @throws IllegalArgumentException if validation fails
         */
        void validatePayment(PaymentDetails paymentDetails);
    }
    
    /**
     * Malaysia compliance module
     */
    private static class MalaysiaComplianceModule implements CountryComplianceModule {
        private static final String COUNTRY = "Malaysia";
        private static final String CURRENCY_CODE = "MYR";
        private static final String REGULATOR = "Securities Commission Malaysia (SC)";
        private static final BigDecimal KYC_THRESHOLD = new BigDecimal("3000");
        
        @Override
        public void validatePayment(PaymentDetails paymentDetails) {
            if (CURRENCY_CODE.equals(paymentDetails.getCurrency())) {
                if (paymentDetails.getAmount().compareTo(KYC_THRESHOLD) >= 0 && 
                        (paymentDetails.getCustomerName() == null || paymentDetails.getCustomerName().isEmpty())) {
                    throw new IllegalArgumentException("KYC information required for payments above " + 
                            KYC_THRESHOLD + " " + CURRENCY_CODE);
                }
            }
        }
    }
    
    /**
     * Singapore compliance module
     */
    private static class SingaporeComplianceModule implements CountryComplianceModule {
        private static final String COUNTRY = "Singapore";
        private static final String CURRENCY_CODE = "SGD";
        private static final String REGULATOR = "Monetary Authority of Singapore (MAS)";
        private static final BigDecimal KYC_THRESHOLD = new BigDecimal("1000");
        private static final BigDecimal TRAVEL_RULE_THRESHOLD = new BigDecimal("1000");
        
        @Override
        public void validatePayment(PaymentDetails paymentDetails) {
            if (CURRENCY_CODE.equals(paymentDetails.getCurrency())) {
                if (paymentDetails.getAmount().compareTo(KYC_THRESHOLD) >= 0 && 
                        (paymentDetails.getCustomerName() == null || paymentDetails.getCustomerName().isEmpty() ||
                         paymentDetails.getCustomerEmail() == null || paymentDetails.getCustomerEmail().isEmpty())) {
                    throw new IllegalArgumentException("KYC information (name and email) required for payments above " + 
                            KYC_THRESHOLD + " " + CURRENCY_CODE);
                }
                
                // Travel Rule compliance
                if (paymentDetails.getAmount().compareTo(TRAVEL_RULE_THRESHOLD) >= 0) {
                    Log.i(TAG, "Travel Rule applies to this transaction");
                    // Additional Travel Rule implementation would go here
                }
            }
        }
    }
    
    /**
     * Indonesia compliance module
     */
    private static class IndonesiaComplianceModule implements CountryComplianceModule {
        private static final String COUNTRY = "Indonesia";
        private static final String CURRENCY_CODE = "IDR";
        private static final String REGULATOR = "Commodity Futures Trading Regulatory Agency (Bappebti)";
        private static final BigDecimal KYC_THRESHOLD = new BigDecimal("2000000");
        
        @Override
        public void validatePayment(PaymentDetails paymentDetails) {
            if (CURRENCY_CODE.equals(paymentDetails.getCurrency())) {
                if (paymentDetails.getAmount().compareTo(KYC_THRESHOLD) >= 0 && 
                        (paymentDetails.getCustomerName() == null || paymentDetails.getCustomerName().isEmpty())) {
                    throw new IllegalArgumentException("KYC information required for payments above " + 
                            KYC_THRESHOLD + " " + CURRENCY_CODE);
                }
            }
        }
    }
    
    /**
     * Thailand compliance module
     */
    private static class ThailandComplianceModule implements CountryComplianceModule {
        private static final String COUNTRY = "Thailand";
        private static final String CURRENCY_CODE = "THB";
        private static final String REGULATOR = "Securities and Exchange Commission (SEC)";
        private static final BigDecimal KYC_THRESHOLD = new BigDecimal("50000");
        
        @Override
        public void validatePayment(PaymentDetails paymentDetails) {
            if (CURRENCY_CODE.equals(paymentDetails.getCurrency())) {
                if (paymentDetails.getAmount().compareTo(KYC_THRESHOLD) >= 0 && 
                        (paymentDetails.getCustomerName() == null || paymentDetails.getCustomerName().isEmpty())) {
                    throw new IllegalArgumentException("KYC information required for payments above " + 
                            KYC_THRESHOLD + " " + CURRENCY_CODE);
                }
            }
        }
    }
    
    /**
     * Brunei compliance module
     */
    private static class BruneiComplianceModule implements CountryComplianceModule {
        private static final String COUNTRY = "Brunei";
        private static final String CURRENCY_CODE = "BND";
        private static final String REGULATOR = "Autoriti Monetari Brunei Darussalam (AMBD)";
        private static final BigDecimal KYC_THRESHOLD = new BigDecimal("5000");
        
        @Override
        public void validatePayment(PaymentDetails paymentDetails) {
            if (CURRENCY_CODE.equals(paymentDetails.getCurrency())) {
                if (paymentDetails.getAmount().compareTo(KYC_THRESHOLD) >= 0 && 
                        (paymentDetails.getCustomerName() == null || paymentDetails.getCustomerName().isEmpty())) {
                    throw new IllegalArgumentException("KYC information required for payments above " + 
                            KYC_THRESHOLD + " " + CURRENCY_CODE);
                }
            }
        }
    }
    
    /**
     * Cambodia compliance module
     */
    private static class CambodiaComplianceModule implements CountryComplianceModule {
        private static final String COUNTRY = "Cambodia";
        private static final String CURRENCY_CODE = "KHR";
        private static final String REGULATOR = "National Bank of Cambodia (NBC)";
        private static final BigDecimal KYC_THRESHOLD = new BigDecimal("4000000");
        
        @Override
        public void validatePayment(PaymentDetails paymentDetails) {
            if (CURRENCY_CODE.equals(paymentDetails.getCurrency())) {
                if (paymentDetails.getAmount().compareTo(KYC_THRESHOLD) >= 0 && 
                        (paymentDetails.getCustomerName() == null || paymentDetails.getCustomerName().isEmpty())) {
                    throw new IllegalArgumentException("KYC information required for payments above " + 
                            KYC_THRESHOLD + " " + CURRENCY_CODE);
                }
            }
        }
    }
    
    /**
     * Vietnam compliance module
     */
    private static class VietnamComplianceModule implements CountryComplianceModule {
        private static final String COUNTRY = "Vietnam";
        private static final String CURRENCY_CODE = "VND";
        private static final String REGULATOR = "State Bank of Vietnam (SBV)";
        private static final BigDecimal KYC_THRESHOLD = new BigDecimal("20000000");
        
        @Override
        public void validatePayment(PaymentDetails paymentDetails) {
            if (CURRENCY_CODE.equals(paymentDetails.getCurrency())) {
                if (paymentDetails.getAmount().compareTo(KYC_THRESHOLD) >= 0 && 
                        (paymentDetails.getCustomerName() == null || paymentDetails.getCustomerName().isEmpty())) {
                    throw new IllegalArgumentException("KYC information required for payments above " + 
                            KYC_THRESHOLD + " " + CURRENCY_CODE);
                }
            }
        }
    }
    
    /**
     * Laos compliance module
     */
    private static class LaosComplianceModule implements CountryComplianceModule {
        private static final String COUNTRY = "Laos";
        private static final String CURRENCY_CODE = "LAK";
        private static final String REGULATOR = "Bank of the Lao PDR (BOL)";
        private static final BigDecimal KYC_THRESHOLD = new BigDecimal("9000000");
        
        @Override
        public void validatePayment(PaymentDetails paymentDetails) {
            if (CURRENCY_CODE.equals(paymentDetails.getCurrency())) {
                if (paymentDetails.getAmount().compareTo(KYC_THRESHOLD) >= 0 && 
                        (paymentDetails.getCustomerName() == null || paymentDetails.getCustomerName().isEmpty())) {
                    throw new IllegalArgumentException("KYC information required for payments above " + 
                            KYC_THRESHOLD + " " + CURRENCY_CODE);
                }
            }
        }
    }
    
    /**
     * Callback interface for payment operations
     */
    public interface PaymentCallback {
        /**
         * Called when payment operation is successful
         * 
         * @param payment Payment object
         */
        void onSuccess(Payment payment);
        
        /**
         * Called when payment operation fails
         * 
         * @param e Exception
         */
        void onError(Exception e);
    }
    
    /**
     * Callback interface for payments list operations
     */
    public interface PaymentsListCallback {
        /**
         * Called when payments list operation is successful
         * 
         * @param payments List of payments
         * @param total Total number of payments
         */
        void onSuccess(List<Payment> payments, int total);
        
        /**
         * Called when payments list operation fails
         * 
         * @param e Exception
         */
        void onError(Exception e);
    }
    
    /**
     * Callback interface for exchange rates operations
     */
    public interface ExchangeRatesCallback {
        /**
         * Called when exchange rates operation is successful
         * 
         * @param baseCurrency Base currency
         * @param rates Map of cryptocurrency to exchange rate
         */
        void onSuccess(String baseCurrency, Map<String, BigDecimal> rates);
        
        /**
         * Called when exchange rates operation fails
         * 
         * @param e Exception
         */
        void onError(Exception e);
    }
    
    /**
     * Callback interface for API requests
     */
    private interface ApiCallback {
        /**
         * Called when API request is successful
         * 
         * @param response JSON response
         */
        void onSuccess(JSONObject response);
        
        /**
         * Called when API request fails
         * 
         * @param e Exception
         */
        void onError(Exception e);
    }
    
    /**
     * Interface for webhook event handlers
     */
    public interface WebhookHandler {
        /**
         * Called when payment is created
         * 
         * @param payment Payment object
         */
        void onPaymentCreated(Payment payment);
        
        /**
         * Called when payment is updated
         * 
         * @param payment Payment object
         */
        void onPaymentUpdated(Payment payment);
        
        /**
         * Called when payment is completed
         * 
         * @param payment Payment object
         */
        void onPaymentCompleted(Payment payment);
        
        /**
         * Called when payment is cancelled
         * 
         * @param payment Payment object
         */
        void onPaymentCancelled(Payment payment);
        
        /**
         * Called when payment is expired
         * 
         * @param payment Payment object
         */
        void onPaymentExpired(Payment payment);
        
        /**
         * Called for unknown event types
         * 
         * @param eventType Event type
         * @param eventData Event data
         */
        void onUnknownEvent(String eventType, JSONObject eventData);
    }
    
    /**
     * Default implementation of WebhookHandler
     */
    public static abstract class DefaultWebhookHandler implements WebhookHandler {
        @Override
        public void onPaymentCreated(Payment payment) {
            // Default implementation does nothing
        }
        
        @Override
        public void onPaymentUpdated(Payment payment) {
            // Default implementation does nothing
        }
        
        @Override
        public void onPaymentCompleted(Payment payment) {
            // Default implementation does nothing
        }
        
        @Override
        public void onPaymentCancelled(Payment payment) {
            // Default implementation does nothing
        }
        
        @Override
        public void onPaymentExpired(Payment payment) {
            // Default implementation does nothing
        }
        
        @Override
        public void onUnknownEvent(String eventType, JSONObject eventData) {
            // Default implementation does nothing
        }
    }
}
