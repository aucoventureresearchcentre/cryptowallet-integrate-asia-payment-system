package country

// CountryModule defines the interface for country-specific regulatory compliance
type CountryModule interface {
	// ValidateTransaction checks if a transaction complies with country-specific regulations
	ValidateTransaction(transaction interface{}) (bool, error)
	
	// GenerateReports creates regulatory reports for the specified time period
	GenerateReports(startTime, endTime interface{}) ([]interface{}, error)
	
	// GetTransactionLimits returns the transaction limits for the country
	GetTransactionLimits() map[string]float64
	
	// CalculateTax calculates any applicable taxes for a transaction
	CalculateTax(transaction interface{}) (float64, error)
	
	// GetSupportedCryptocurrencies returns the list of supported cryptocurrencies
	GetSupportedCryptocurrencies() []string
	
	// GetCountryCode returns the ISO country code
	GetCountryCode() string
	
	// GetCountryName returns the country name
	GetCountryName() string
	
	// GetRegulators returns the list of regulatory authorities
	GetRegulators() []string
	
	// GetKYCRequirements returns the KYC requirements for the country
	GetKYCRequirements() map[string]interface{}
}

// BaseCountryModule provides common functionality for all country modules
type BaseCountryModule struct {
	CountryCode string
	CountryName string
	Regulators  []string
}

// GetCountryCode returns the ISO country code
func (m *BaseCountryModule) GetCountryCode() string {
	return m.CountryCode
}

// GetCountryName returns the country name
func (m *BaseCountryModule) GetCountryName() string {
	return m.CountryName
}

// GetRegulators returns the list of regulatory authorities
func (m *BaseCountryModule) GetRegulators() []string {
	return m.Regulators
}

// CountryModuleRegistry maintains a registry of country modules
type CountryModuleRegistry struct {
	modules map[string]CountryModule
}

// NewCountryModuleRegistry creates a new registry
func NewCountryModuleRegistry() *CountryModuleRegistry {
	return &CountryModuleRegistry{
		modules: make(map[string]CountryModule),
	}
}

// RegisterModule adds a country module to the registry
func (r *CountryModuleRegistry) RegisterModule(module CountryModule) {
	r.modules[module.GetCountryCode()] = module
}

// GetModule retrieves a country module by country code
func (r *CountryModuleRegistry) GetModule(countryCode string) (CountryModule, bool) {
	module, exists := r.modules[countryCode]
	return module, exists
}

// GetAllModules returns all registered country modules
func (r *CountryModuleRegistry) GetAllModules() map[string]CountryModule {
	return r.modules
}
