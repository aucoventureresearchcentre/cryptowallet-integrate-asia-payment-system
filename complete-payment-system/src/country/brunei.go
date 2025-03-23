package brunei

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

// BruneiModule implements country-specific regulatory compliance for Brunei
type BruneiModule struct {
	CountryCode string
	CountryName string
	Regulators  []string
	
	// Brunei-specific fields
	AMBDNotified bool // Whether the merchant has notified Autoriti Monetari Brunei Darussalam
}

// NewBruneiModule creates a new instance of BruneiModule
func NewBruneiModule() *BruneiModule {
	return &BruneiModule{
		CountryCode: "BN",
		CountryName: "Brunei Darussalam",
		Regulators: []string{
			"Autoriti Monetari Brunei Darussalam (AMBD)",
		},
		AMBDNotified: false, // Default to false, should be set based on merchant notification status
	}
}

// ValidateTransaction checks if a transaction complies with Brunei regulations
func (m *BruneiModule) ValidateTransaction(transaction interface{}) (bool, error) {
	tx, ok := transaction.(*Transaction)
	if !ok {
		return false, errors.New("invalid transaction type")
	}
	
	// Check if the merchant has notified AMBD
	if !m.AMBDNotified {
		return false, errors.New("merchant has not notified Autoriti Monetari Brunei Darussalam")
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
		return false, errors.New("cryptocurrency not supported in Brunei")
	}
	
	// All checks passed
	return true, nil
}

// GenerateReports creates regulatory reports for the specified time period
func (m *BruneiModule) GenerateReports(startTime, endTime interface{}) ([]interface{}, error) {
	// In a real implementation, we would:
	// 1. Query transactions for the specified time period
	// 2. Format the data according to AMBD requirements
	// 3. Generate the required reports
	
	// For now, we'll return a placeholder
	return []interface{}{
		map[string]string{
			"report_type": "AMBD_Notification",
			"period": "2025-03",
			"status": "generated",
		},
	}, nil
}

// GetTransactionLimits returns the transaction limits for Brunei
func (m *BruneiModule) GetTransactionLimits() map[string]float64 {
	// Brunei has limited specific regulations, so these are conservative estimates
	return map[string]float64{
		"daily":   50000.0,  // 50,000 BND per day
		"monthly": 500000.0, // 500,000 BND per month
	}
}

// CalculateTax calculates applicable taxes for a transaction in Brunei
func (m *BruneiModule) CalculateTax(transaction interface{}) (float64, error) {
	// Brunei does not have a capital gains tax or income tax on cryptocurrency
	// Returns 0 tax amount
	return 0.0, nil
}

// GetSupportedCryptocurrencies returns the list of cryptocurrencies supported in Brunei
func (m *BruneiModule) GetSupportedCryptocurrencies() []string {
	// Brunei has no official list, so this is based on commonly accepted cryptocurrencies
	return []string{
		"BTC",  // Bitcoin
		"ETH",  // Ethereum
		"USDT", // Tether
		"BNB",  // Binance Coin
	}
}

// GetCountryCode returns the ISO country code for Brunei
func (m *BruneiModule) GetCountryCode() string {
	return m.CountryCode
}

// GetCountryName returns the country name
func (m *BruneiModule) GetCountryName() string {
	return m.CountryName
}

// GetRegulators returns the list of regulatory authorities in Brunei
func (m *BruneiModule) GetRegulators() []string {
	return m.Regulators
}

// GetKYCRequirements returns the KYC requirements for Brunei
func (m *BruneiModule) GetKYCRequirements() map[string]interface{} {
	return map[string]interface{}{
		"individual": []string{
			"Full Name",
			"National ID or Passport",
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
		"transaction_threshold": 10000.0, // BND, threshold for enhanced due diligence
	}
}

// GenerateWarningNotice creates a warning notice for users as required by AMBD
func (m *BruneiModule) GenerateWarningNotice() string {
	return `WARNING: Cryptocurrencies are not legal tender in Brunei Darussalam and are not regulated by Autoriti Monetari Brunei Darussalam (AMBD). Investments in cryptocurrency products involve risks including the potential loss of principal. Investors should be aware that cryptocurrency values can fluctuate and past performance is not indicative of future results.`
}
