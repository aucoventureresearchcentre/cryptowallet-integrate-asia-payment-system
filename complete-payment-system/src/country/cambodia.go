package cambodia

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

// CambodiaModule implements country-specific regulatory compliance for Cambodia
type CambodiaModule struct {
	CountryCode string
	CountryName string
	Regulators  []string
	
	// Cambodia-specific fields
	NBCLicensed bool // Whether the merchant is licensed by National Bank of Cambodia
	CryptoGroup string // Group classification of cryptoassets (Group 1a, 1b, or 2)
}

// NewCambodiaModule creates a new instance of CambodiaModule
func NewCambodiaModule() *CambodiaModule {
	return &CambodiaModule{
		CountryCode: "KH",
		CountryName: "Cambodia",
		Regulators: []string{
			"National Bank of Cambodia (NBC)",
		},
		NBCLicensed: false, // Default to false, should be set based on merchant license status
		CryptoGroup: "",    // Should be set based on cryptoasset classification
	}
}

// ValidateTransaction checks if a transaction complies with Cambodian regulations
func (m *CambodiaModule) ValidateTransaction(transaction interface{}) (bool, error) {
	tx, ok := transaction.(*Transaction)
	if !ok {
		return false, errors.New("invalid transaction type")
	}
	
	// Check if the merchant is licensed by NBC
	if !m.NBCLicensed {
		return false, errors.New("merchant is not licensed by National Bank of Cambodia")
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
		return false, errors.New("cryptocurrency not supported in Cambodia")
	}
	
	// Check cryptoasset group restrictions
	group, err := m.ClassifyCryptoAsset(tx.CryptoCurrency)
	if err != nil {
		return false, err
	}
	
	// Commercial banks may only provide services for Group 1 cryptoassets
	if m.CryptoGroup == "commercial_bank" && group == "Group 2" {
		return false, errors.New("commercial banks may not provide services for Group 2 cryptoassets")
	}
	
	// All checks passed
	return true, nil
}

// GenerateReports creates regulatory reports for the specified time period
func (m *CambodiaModule) GenerateReports(startTime, endTime interface{}) ([]interface{}, error) {
	// In a real implementation, we would:
	// 1. Query transactions for the specified time period
	// 2. Format the data according to NBC requirements
	// 3. Generate the required reports
	
	// For now, we'll return a placeholder
	return []interface{}{
		map[string]string{
			"report_type": "NBC_Quarterly_Report",
			"period": "2025-Q1",
			"status": "generated",
		},
	}, nil
}

// GetTransactionLimits returns the transaction limits for Cambodia
func (m *CambodiaModule) GetTransactionLimits() map[string]float64 {
	return map[string]float64{
		"daily":   40000000.0,  // 40,000,000 KHR per day (approx. $10,000 USD)
		"monthly": 400000000.0, // 400,000,000 KHR per month (approx. $100,000 USD)
	}
}

// CalculateTax calculates applicable taxes for a transaction in Cambodia
func (m *CambodiaModule) CalculateTax(transaction interface{}) (float64, error) {
	tx, ok := transaction.(*Transaction)
	if !ok {
		return 0, errors.New("invalid transaction type")
	}
	
	// In Cambodia, cryptocurrency transactions may be subject to income tax
	// at the standard rate of 20%
	taxRate := 0.20
	
	return tx.Amount * taxRate, nil
}

// GetSupportedCryptocurrencies returns the list of cryptocurrencies supported in Cambodia
func (m *CambodiaModule) GetSupportedCryptocurrencies() []string {
	// Based on NBC's regulations, though no official list exists
	return []string{
		"BTC",  // Bitcoin
		"ETH",  // Ethereum
		"USDT", // Tether
		"USDC", // USD Coin
		"BNB",  // Binance Coin
	}
}

// GetCountryCode returns the ISO country code for Cambodia
func (m *CambodiaModule) GetCountryCode() string {
	return m.CountryCode
}

// GetCountryName returns the country name
func (m *CambodiaModule) GetCountryName() string {
	return m.CountryName
}

// GetRegulators returns the list of regulatory authorities in Cambodia
func (m *CambodiaModule) GetRegulators() []string {
	return m.Regulators
}

// GetKYCRequirements returns the KYC requirements for Cambodia
func (m *CambodiaModule) GetKYCRequirements() map[string]interface{} {
	return map[string]interface{}{
		"individual": []string{
			"Full Name",
			"National ID or Passport",
			"Date of Birth",
			"Residential Address",
			"Contact Information",
			"Source of Funds",
			"Occupation",
		},
		"business": []string{
			"Business Name",
			"Business Registration Number",
			"Business Address",
			"Director Information",
			"Shareholder Information",
			"Source of Funds",
			"Business Activities",
		},
		"transaction_threshold": 4000000.0, // KHR, threshold for enhanced due diligence (approx. $1,000 USD)
	}
}

// ClassifyCryptoAsset classifies a cryptocurrency according to NBC regulations
func (m *CambodiaModule) ClassifyCryptoAsset(cryptoCurrency string) (string, error) {
	// In Cambodia, cryptoassets are classified into groups:
	// Group 1: Digital representation of traditional financial instruments or assets
	//   Group 1a: Tokenized securities
	//   Group 1b: Stablecoins
	// Group 2: All cryptoassets that don't meet Group 1 criteria (unbacked cryptoassets)
	
	group1b := []string{"USDT", "USDC", "BUSD", "DAI"}
	for _, crypto := range group1b {
		if crypto == cryptoCurrency {
			return "Group 1b", nil
		}
	}
	
	group1a := []string{"LINK", "UNI", "AAVE"}
	for _, crypto := range group1a {
		if crypto == cryptoCurrency {
			return "Group 1a", nil
		}
	}
	
	// Check if it's a supported cryptocurrency
	supported := m.GetSupportedCryptocurrencies()
	for _, crypto := range supported {
		if crypto == cryptoCurrency {
			return "Group 2", nil
		}
	}
	
	return "", errors.New("unsupported cryptocurrency")
}

// CalculateExposureLimits calculates the exposure limits for a commercial bank
func (m *CambodiaModule) CalculateExposureLimits(cet1Capital float64) map[string]float64 {
	// According to NBC regulations:
	// - Group 1a exposures must not exceed 5% of CET1 Capital
	// - Group 1b exposures must not exceed 3% of CET1 Capital
	
	return map[string]float64{
		"Group 1a": cet1Capital * 0.05,
		"Group 1b": cet1Capital * 0.03,
		"Group 2":  0.0, // Commercial banks cannot have Group 2 exposures
	}
}
