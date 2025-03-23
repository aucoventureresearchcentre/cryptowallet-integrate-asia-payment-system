package malaysia

import (
	"errors"
	"time"
)

// Transaction represents a cryptocurrency payment transaction
type Transaction struct {
	ID               string
	Amount           float64
	Currency         string
	CryptoCurrency   string
	SourceAddress    string
	DestinationAddress string
	Status           string
	CreatedAt        time.Time
	UpdatedAt        time.Time
	CompletedAt      time.Time
	MerchantID       string
	CustomerID       string
	CountryCode      string
}

// MalaysiaModule implements country-specific regulatory compliance for Malaysia
type MalaysiaModule struct {
	CountryCode string
	CountryName string
	Regulators  []string
	
	// Malaysia-specific fields
	SCLicensed bool // Whether the merchant is licensed by Securities Commission Malaysia
}

// NewMalaysiaModule creates a new instance of MalaysiaModule
func NewMalaysiaModule() *MalaysiaModule {
	return &MalaysiaModule{
		CountryCode: "MY",
		CountryName: "Malaysia",
		Regulators: []string{
			"Securities Commission Malaysia (SC)",
			"Bank Negara Malaysia (BNM)",
		},
		SCLicensed: false, // Default to false, should be set based on merchant registration
	}
}

// ValidateTransaction checks if a transaction complies with Malaysian regulations
func (m *MalaysiaModule) ValidateTransaction(transaction interface{}) (bool, error) {
	tx, ok := transaction.(*Transaction)
	if !ok {
		return false, errors.New("invalid transaction type")
	}
	
	// Check if the merchant is licensed by SC (required for crypto trading in Malaysia)
	if !m.SCLicensed {
		return false, errors.New("merchant is not licensed by Securities Commission Malaysia")
	}
	
	// Check transaction limits
	limits := m.GetTransactionLimits()
	if tx.Amount > limits["daily"] {
		return false, errors.New("transaction exceeds daily limit")
	}
	
	// Check if cryptocurrency is supported
	supported := m.GetSupportedCryptocurrencies()
	cryptoSupported := false
	for _, crypto := range supported {
		if crypto == tx.CryptoCurrency {
			cryptoSupported = true
			break
		}
	}
	
	if !cryptoSupported {
		return false, errors.New("cryptocurrency not supported in Malaysia")
	}
	
	// All checks passed
	return true, nil
}

// GenerateReports creates regulatory reports for the specified time period
func (m *MalaysiaModule) GenerateReports(startTime, endTime interface{}) ([]interface{}, error) {
	// In a real implementation, we would:
	// 1. Query transactions for the specified time period
	// 2. Format the data according to SC and BNM requirements
	// 3. Generate the required reports
	
	// For now, we'll return a placeholder
	return []interface{}{
		map[string]string{
			"report_type": "SC_Monthly_Report",
			"period": "2025-03",
			"status": "generated",
		},
	}, nil
}

// GetTransactionLimits returns the transaction limits for Malaysia
func (m *MalaysiaModule) GetTransactionLimits() map[string]float64 {
	return map[string]float64{
		"daily":   50000.0,  // 50,000 MYR per day
		"monthly": 500000.0, // 500,000 MYR per month
	}
}

// CalculateTax calculates applicable taxes for a transaction in Malaysia
func (m *MalaysiaModule) CalculateTax(transaction interface{}) (float64, error) {
	tx, ok := transaction.(*Transaction)
	if !ok {
		return 0, errors.New("invalid transaction type")
	}
	
	// In Malaysia, capital gains from cryptocurrency are subject to income tax
	// if the activity is deemed to be trading in nature
	// For simplicity, we'll assume a flat rate of 24% (corporate tax rate)
	taxRate := 0.24
	
	return tx.Amount * taxRate, nil
}

// GetSupportedCryptocurrencies returns the list of cryptocurrencies supported in Malaysia
func (m *MalaysiaModule) GetSupportedCryptocurrencies() []string {
	// Based on Securities Commission Malaysia's approved digital assets
	return []string{
		"BTC",  // Bitcoin
		"ETH",  // Ethereum
		"XRP",  // Ripple
		"BCH",  // Bitcoin Cash
		"LTC",  // Litecoin
		"BNB",  // Binance Coin
		"USDT", // Tether
	}
}

// GetCountryCode returns the ISO country code for Malaysia
func (m *MalaysiaModule) GetCountryCode() string {
	return m.CountryCode
}

// GetCountryName returns the country name
func (m *MalaysiaModule) GetCountryName() string {
	return m.CountryName
}

// GetRegulators returns the list of regulatory authorities in Malaysia
func (m *MalaysiaModule) GetRegulators() []string {
	return m.Regulators
}

// GetKYCRequirements returns the KYC requirements for Malaysia
func (m *MalaysiaModule) GetKYCRequirements() map[string]interface{} {
	return map[string]interface{}{
		"individual": []string{
			"Full Name",
			"National ID Number (MyKad)",
			"Date of Birth",
			"Residential Address",
			"Contact Information",
			"Source of Funds",
		},
		"business": []string{
			"Business Name",
			"Business Registration Number",
			"Business Address",
			"Director Information",
			"Shareholder Information",
			"Source of Funds",
		},
		"transaction_threshold": 3000.0, // MYR, threshold for enhanced due diligence
	}
}
