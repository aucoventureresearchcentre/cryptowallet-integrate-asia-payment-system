package com.asiancryptopay.example;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.asiancryptopay.sdk.AsianCryptoPayment;

import org.json.JSONException;
import org.json.JSONObject;

import java.text.DecimalFormat;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Example POS Terminal application integrating the Asian Cryptocurrency Payment System
 */
public class POSTerminalActivity extends AppCompatActivity {

    private AsianCryptoPayment paymentSDK;
    private String selectedCountry = "MY";
    private String selectedCurrency = "MYR";
    private String selectedCryptoCurrency = "BTC";
    private double amount = 0.0;
    
    // UI Elements
    private Spinner countrySpinner;
    private Spinner cryptoSpinner;
    private EditText amountInput;
    private Button createPaymentButton;
    private LinearLayout paymentDetailsLayout;
    private TextView statusTextView;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pos_terminal);
        
        // Initialize UI elements
        countrySpinner = findViewById(R.id.country_spinner);
        cryptoSpinner = findViewById(R.id.crypto_spinner);
        amountInput = findViewById(R.id.amount_input);
        createPaymentButton = findViewById(R.id.create_payment_button);
        paymentDetailsLayout = findViewById(R.id.payment_details_layout);
        statusTextView = findViewById(R.id.status_text);
        
        // Set up country spinner
        setupCountrySpinner();
        
        // Set up cryptocurrency spinner
        setupCryptoSpinner();
        
        // Initialize payment SDK
        initializePaymentSDK();
        
        // Set up create payment button
        createPaymentButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                createPayment();
            }
        });
    }
    
    /**
     * Set up the country spinner with supported countries
     */
    private void setupCountrySpinner() {
        // Country names with flags
        final String[] countries = {
                "🇲🇾 Malaysia", 
                "🇸🇬 Singapore", 
                "🇮🇩 Indonesia", 
                "🇹🇭 Thailand", 
                "🇧🇳 Brunei", 
                "🇰🇭 Cambodia", 
                "🇻🇳 Vietnam", 
                "🇱🇦 Laos"
        };
        
        // Country codes
        final String[] countryCodes = {"MY", "SG", "ID", "TH", "BN", "KH", "VN", "LA"};
        
        ArrayAdapter<String> adapter = new ArrayAdapter<>(
                this, android.R.layout.simple_spinner_item, countries);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        countrySpinner.setAdapter(adapter);
        
        countrySpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                selectedCountry = countryCodes[position];
                selectedCurrency = getDefaultCurrencyForCountry(selectedCountry);
                
                // Reinitialize the payment SDK with the new country
                initializePaymentSDK();
                
                // Update UI to show selected currency
                updateCurrencyDisplay();
            }
            
            @Override
            public void onNothingSelected(AdapterView<?> parent) {
                // Do nothing
            }
        });
    }
    
    /**
     * Set up the cryptocurrency spinner with supported cryptocurrencies
     */
    private void setupCryptoSpinner() {
        final String[] cryptocurrencies = {
                "Bitcoin (BTC)", 
                "Ethereum (ETH)", 
                "Tether (USDT)"
        };
        
        final String[] cryptoCodes = {"BTC", "ETH", "USDT"};
        
        ArrayAdapter<String> adapter = new ArrayAdapter<>(
                this, android.R.layout.simple_spinner_item, cryptocurrencies);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        cryptoSpinner.setAdapter(adapter);
        
        cryptoSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                selectedCryptoCurrency = cryptoCodes[position];
            }
            
            @Override
            public void onNothingSelected(AdapterView<?> parent) {
                // Do nothing
            }
        });
    }
    
    /**
     * Initialize the payment SDK with the selected country
     */
    private void initializePaymentSDK() {
        try {
            paymentSDK = new AsianCryptoPayment(
                    "demo_api_key",
                    "demo_merchant_id",
                    selectedCountry,
                    "https://api.asiancryptopay.com/v1",
                    true,
                    selectedCurrency,
                    Arrays.asList("BTC", "ETH", "USDT")
            );
            
            // Get supported cryptocurrencies for the selected country
            paymentSDK.getSupportedCryptocurrencies(new AsianCryptoPayment.PaymentCallback() {
                @Override
                public void onSuccess(JSONObject response) {
                    // In a real implementation, we would update the crypto spinner
                    // with the supported cryptocurrencies for the selected country
                }
                
                @Override
                public void onError(Exception e) {
                    showError("Failed to get supported cryptocurrencies: " + e.getMessage());
                }
            });
            
        } catch (Exception e) {
            showError("Failed to initialize payment SDK: " + e.getMessage());
        }
    }
    
    /**
     * Create a new payment
     */
    private void createPayment() {
        // Get amount from input
        String amountStr = amountInput.getText().toString().trim();
        if (amountStr.isEmpty()) {
            showError("Please enter an amount");
            return;
        }
        
        try {
            amount = Double.parseDouble(amountStr);
            if (amount <= 0) {
                showError("Amount must be greater than zero");
                return;
            }
        } catch (NumberFormatException e) {
            showError("Invalid amount");
            return;
        }
        
        // Show loading state
        statusTextView.setText("Creating payment...");
        paymentDetailsLayout.setVisibility(View.VISIBLE);
        
        // Create payment
        Map<String, Object> metadata = new HashMap<>();
        metadata.put("posTerminalId", "POS-" + Math.floor(Math.random() * 1000));
        metadata.put("merchantLocation", "Store #123");
        
        paymentSDK.createPayment(
                amount,
                selectedCurrency,
                selectedCryptoCurrency,
                "ORD-" + Math.floor(Math.random() * 1000000),
                "POS Terminal Payment",
                null,
                metadata,
                new AsianCryptoPayment.PaymentCallback() {
                    @Override
                    public void onSuccess(final JSONObject payment) {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                try {
                                    // Display payment details
                                    String paymentAddress = payment.getString("payment_address");
                                    String transactionId = payment.getString("transaction_id");
                                    double cryptoAmount = payment.getDouble("crypto_amount");
                                    String cryptoCurrency = payment.getString("crypto_currency");
                                    String expiresAt = payment.getString("expires_at");
                                    
                                    // Update UI
                                    TextView addressTextView = findViewById(R.id.payment_address);
                                    TextView amountTextView = findViewById(R.id.crypto_amount);
                                    TextView expiryTextView = findViewById(R.id.payment_expiry);
                                    
                                    addressTextView.setText("Address: " + paymentAddress);
                                    amountTextView.setText("Amount: " + new DecimalFormat("#.########").format(cryptoAmount) + " " + cryptoCurrency);
                                    expiryTextView.setText("Expires: " + expiresAt);
                                    statusTextView.setText("Waiting for payment...");
                                    
                                    // In a real implementation, we would display a QR code
                                    // For this example, we'll just show a message
                                    TextView qrTextView = findViewById(R.id.qr_placeholder);
                                    qrTextView.setText("QR Code for: " + paymentAddress);
                                    
                                    // Start checking payment status
                                    startCheckingPaymentStatus(transactionId);
                                    
                                } catch (JSONException e) {
                                    showError("Error parsing payment response: " + e.getMessage());
                                }
                            }
                        });
                    }
                    
                    @Override
                    public void onError(final Exception e) {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                showError("Payment creation failed: " + e.getMessage());
                            }
                        });
                    }
                }
        );
    }
    
    /**
     * Start checking payment status periodically
     */
    private void startCheckingPaymentStatus(final String transactionId) {
        // In a real implementation, we would check the status periodically
        // For this example, we'll simulate a successful payment after 5 seconds
        
        new android.os.Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        statusTextView.setText("Payment completed successfully!");
                        
                        // Show success message
                        Toast.makeText(POSTerminalActivity.this, 
                                "Payment received! Transaction ID: " + transactionId, 
                                Toast.LENGTH_LONG).show();
                        
                        // In a real implementation, we would update the order status
                        // and proceed with order fulfillment
                    }
                });
            }
        }, 5000);
    }
    
    /**
     * Update the currency display based on the selected country
     */
    private void updateCurrencyDisplay() {
        TextView currencyLabel = findViewById(R.id.currency_label);
        currencyLabel.setText("Amount (" + selectedCurrency + "):");
    }
    
    /**
     * Get the default currency for a country
     */
    private String getDefaultCurrencyForCountry(String countryCode) {
        Map<String, String> currencyMap = new HashMap<>();
        currencyMap.put("MY", "MYR"); // Malaysian Ringgit
        currencyMap.put("SG", "SGD"); // Singapore Dollar
        currencyMap.put("ID", "IDR"); // Indonesian Rupiah
        currencyMap.put("TH", "THB"); // Thai Baht
        currencyMap.put("BN", "BND"); // Brunei Dollar
        currencyMap.put("KH", "KHR"); // Cambodian Riel
        currencyMap.put("VN", "VND"); // Vietnamese Dong
        currencyMap.put("LA", "LAK"); // Lao Kip
        
        return currencyMap.containsKey(countryCode) ? currencyMap.get(countryCode) : "USD";
    }
    
    /**
     * Show an error message
     */
    private void showError(String message) {
        Toast.makeText(this, message, Toast.LENGTH_LONG).show();
        statusTextView.setText("Error: " + message);
    }
}
