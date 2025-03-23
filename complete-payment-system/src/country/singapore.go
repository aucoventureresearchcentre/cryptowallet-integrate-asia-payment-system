package singapore

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

// SingaporeModule implements country-specific regulatory compliance for Singapore
type SingaporeModule struct {
	CountryCode string
	CountryName string
	Regulators  []string
	
	// Singapore-specific fields
	MASLicensed bool // Whether the merchant is licensed by Monetary Authority of Singapore
	PSACompliant bool // Whether the merchant complies with Payment Services Act
}

// NewSingaporeModule creates a new instance of SingaporeModule
func NewSingaporeModule() *SingaporeModule {
	return &SingaporeModule{
		CountryCode: "SG",
		CountryName: "Singapore",
		Regulators: []string{
			"Monetary Authority of Singapore (MAS)",
		},
		MASLicensed: false, // Default to false, should be set based on merchant registration
		PSACompliant: false, // Default to false, should be set based on merchant compliance
	}
}

// ValidateTransaction checks if a transaction complies with Singapore regulations
func (m *SingaporeModule) ValidateTransaction(transaction interface{}) (bool, error) {
	tx, ok := transaction.(*Transaction)
	if !ok {
		return false, errors.New("invalid transaction type")
	}
	
	// Check if the merchant is licensed by MAS (required for digital payment token services)
	if !m.MASLicensed {
		return false, errors.New("merchant is not licensed by Monetary Authority of Singapore")
	}
	
	// Check if the merchant complies with Payment Services Act
	if !m.PSACompliant {
		return false, errors.New("merchant does not comply with Payment Services Act requirements")
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
		return false, errors.New("cryptocurrency not supported in Singapore")
	}
	
	// All checks passed
	return true, nil
}

// GenerateReports creates regulatory reports for the specified time period
func (m *SingaporeModule) GenerateReports(startTime, endTime interface{}) ([]interface{}, error) {
	// In a real implementation, we would:
	// 1. Query transactions for the specified time period
	// 2. Format the data according to MAS requirements
	// 3. Generate the required reports including Travel Rule information
	
	// For now, we'll return a placeholder
	return []interface{}{
		map[string]string{
			"report_type": "MAS_Quarterly_Report",
			"period": "2025-Q1",
			"status": "generated",
		},
		map[string]string{
			"report_type": "Travel_Rule_Compliance_Report",
			"period": "2025-03",
			"status": "generated",
		},
	}, nil
}

// GetTransactionLimits returns the transaction limits for Singapore
func (m *SingaporeModule) GetTransactionLimits() map[string]float64 {
	return map[string]float64{
		"daily":   100000.0, // 100,000 SGD per day
		"monthly": 1000000.0, // 1,000,000 SGD per month
	}
}

// CalculateTax calculates applicable taxes for a transaction in Singapore
func (m *SingaporeModule) CalculateTax(transaction interface{}) (float64, error) {
	tx, ok := transaction.(*Transaction)
	if !ok {
		return 0, errors.New("invalid transaction type")
	}
	
	// In Singapore, cryptocurrency is treated as an intangible property
	// and GST (Goods and Services Tax) applies at 8% (as of 2025)
	taxRate := 0.08
	
	return tx.Amount * taxRate, nil
}

// GetSupportedCryptocurrencies returns the list of cryptocurrencies supported in Singapore
func (m *SingaporeModule) GetSupportedCryptocurrencies() []string {
	// Based on MAS Payment Services Act regulated digital payment tokens
	return []string{
		"BTC",  // Bitcoin
		"ETH",  // Ethereum
		"XRP",  // Ripple
		"LTC",  // Litecoin
		"BCH",  // Bitcoin Cash
		"USDT", // Tether
		"USDC", // USD Coin
		"BNB",  // Binance Coin
		"SOL",  // Solana
		"ADA",  // Cardano
	}
}

// GetCountryCode returns the ISO country code for Singapore
func (m *SingaporeModule) GetCountryCode() string {
	return m.CountryCode
}

// GetCountryName returns the country name
func (m *SingaporeModule) GetCountryName() string {
	return m.CountryName
}

// GetRegulators returns the list of regulatory authorities in Singapore
func (m *SingaporeModule) GetRegulators() []string {
	return m.Regulators
}

// GetKYCRequirements returns the KYC requirements for Singapore
func (m *SingaporeModule) GetKYCRequirements() map[string]interface{} {
	return map[string]interface{}{
		"individual": []string{
			"Full Name",
			"NRIC or Passport Number",
			"Date of Birth",
			"Nationality",
			"Residential Address",
			"Contact Information",
			"Source of Funds",
			"Occupation",
		},
		"business": []string{
			"Business Name",
			"Unique Entity Number (UEN)",
			"Business Address",
			"Business Type",
			"Director Information",
			"Shareholder Information",
			"Source of Funds",
			"Business Activities",
		},
		"transaction_threshold": 5000.0, // SGD, threshold for enhanced due diligence
		"travel_rule_threshold": 1500.0, // SGD, threshold for Travel Rule information
	}
}

// GenerateTravelRuleData creates Travel Rule information for a transaction
func (m *SingaporeModule) GenerateTravelRuleData(transaction interface{}) (map[string]interface{}, error) {
	tx, ok := transaction.(*Transaction)
	if !ok {
		return nil, errors.New("invalid transaction type")
	}
	
	// Check if Travel Rule applies (threshold is 1,500 SGD)
	if tx.Currency == "SGD" && tx.Amount < 1500.0 {
		return nil, nil // Travel Rule does not apply
	}
	
	// In a real implementation, we would:
	// 1. Collect originator information
	// 2. Collect beneficiary information
	// 3. Format according to Travel Rule requirements
	
	// For now, we'll return a placeholder
	return map[string]interface{}{
		"originator": map[string]string{
			"name": "Example Originator",
			"account_number": tx.SourceAddress,
			"address": "123 Example Street, Singapore",
		},
		"beneficiary": map[string]string{
			"name": "Example Beneficiary",
			"account_number": tx.DestinationAddress,
		},
		"transaction_hash": tx.ID,
		"amount": tx.Amount,
		"currency": tx.Currency,
		"timestamp": tx.CreatedAt.Format(time.RFC3339),
	}, nil
}
