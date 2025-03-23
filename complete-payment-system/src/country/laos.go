package laos

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

// LaosModule implements country-specific regulatory compliance for Laos
type LaosModule struct {
	CountryCode string
	CountryName string
	Regulators  []string
	
	// Laos-specific fields
	BOLLicensed bool // Whether the merchant is licensed by Bank of the Lao PDR
	PilotParticipant bool // Whether the merchant is part of the pilot program
}

// NewLaosModule creates a new instance of LaosModule
func NewLaosModule() *LaosModule {
	return &LaosModule{
		CountryCode: "LA",
		CountryName: "Lao People's Democratic Republic",
		Regulators: []string{
			"Bank of the Lao PDR (BOL)",
		},
		BOLLicensed: false, // Default to false, should be set based on merchant license status
		PilotParticipant: false, // Default to false, should be set based on pilot program participation
	}
}

// ValidateTransaction checks if a transaction complies with Laotian regulations
func (m *LaosModule) ValidateTransaction(transaction interface{}) (bool, error) {
	tx, ok := transaction.(*Transaction)
	if !ok {
		return false, errors.New("invalid transaction type")
	}
	
	// Check if the merchant is licensed by BOL
	if !m.BOLLicensed {
		return false, errors.New("merchant is not licensed by Bank of the Lao PDR")
	}
	
	// Check if the merchant is part of the pilot program
	if !m.PilotParticipant {
		return false, errors.New("merchant is not part of the cryptocurrency pilot program")
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
		return false, errors.New("cryptocurrency not supported in Laos")
	}
	
	// All checks passed
	return true, nil
}

// GenerateReports creates regulatory reports for the specified time period
func (m *LaosModule) GenerateReports(startTime, endTime interface{}) ([]interface{}, error) {
	// In a real implementation, we would:
	// 1. Query transactions for the specified time period
	// 2. Format the data according to BOL requirements
	// 3. Generate the required reports
	
	// For now, we'll return a placeholder
	return []interface{}{
		map[string]string{
			"report_type": "BOL_Pilot_Program_Report",
			"period": "2025-03",
			"status": "generated",
		},
	}, nil
}

// GetTransactionLimits returns the transaction limits for Laos
func (m *LaosModule) GetTransactionLimits() map[string]float64 {
	return map[string]float64{
		"daily":   50000000.0,  // 50,000,000 LAK per day (approx. $2,500 USD)
		"monthly": 500000000.0, // 500,000,000 LAK per month (approx. $25,000 USD)
	}
}

// CalculateTax calculates applicable taxes for a transaction in Laos
func (m *LaosModule) CalculateTax(transaction interface{}) (float64, error) {
	tx, ok := transaction.(*Transaction)
	if !ok {
		return 0, errors.New("invalid transaction type")
	}
	
	// In Laos, cryptocurrency transactions may be subject to income tax
	// at the standard rate of 24%
	taxRate := 0.24
	
	return tx.Amount * taxRate, nil
}

// GetSupportedCryptocurrencies returns the list of cryptocurrencies supported in Laos
func (m *LaosModule) GetSupportedCryptocurrencies() []string {
	// Based on the pilot program, though no official list exists
	return []string{
		"BTC",  // Bitcoin
		"ETH",  // Ethereum
		"USDT", // Tether
	}
}

// GetCountryCode returns the ISO country code for Laos
func (m *LaosModule) GetCountryCode() string {
	return m.CountryCode
}

// GetCountryName returns the country name
func (m *LaosModule) GetCountryName() string {
	return m.CountryName
}

// GetRegulators returns the list of regulatory authorities in Laos
func (m *LaosModule) GetRegulators() []string {
	return m.Regulators
}

// GetKYCRequirements returns the KYC requirements for Laos
func (m *LaosModule) GetKYCRequirements() map[string]interface{} {
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
		"transaction_threshold": 10000000.0, // LAK, threshold for enhanced due diligence (approx. $500 USD)
	}
}

// CheckPilotStatus checks if the pilot program is still active
func (m *LaosModule) CheckPilotStatus() (bool, error) {
	// The pilot program in Laos started in September 2021 and was set to last three years
	// Check if we're still within the pilot period or if it has been extended
	
	pilotStartDate := time.Date(2021, time.September, 9, 0, 0, 0, 0, time.UTC)
	originalEndDate := pilotStartDate.AddDate(3, 0, 0) // 3 years from start date
	extendedEndDate := time.Date(2025, time.December, 31, 0, 0, 0, 0, time.UTC) // Extended to end of 2025
	
	currentTime := time.Now()
	
	if currentTime.After(extendedEndDate) {
		return false, errors.New("the cryptocurrency pilot program in Laos has ended")
	}
	
	if currentTime.After(originalEndDate) {
		return true, nil // Still active under the extension
	}
	
	return true, nil // Still active under the original pilot period
}

// CheckActivityStatus checks if the merchant's operations are active
func (m *LaosModule) CheckActivityStatus(lastActivityDate time.Time) (bool, error) {
	// In November 2023, Laos announced it would suspend business operations of companies
	// that fail to make progress or fulfill agreements
	
	currentTime := time.Now()
	inactivityThreshold := 90 * 24 * time.Hour // 90 days
	
	if currentTime.Sub(lastActivityDate) > inactivityThreshold {
		return false, errors.New("merchant may be subject to suspension due to inactivity")
	}
	
	return true, nil
}
