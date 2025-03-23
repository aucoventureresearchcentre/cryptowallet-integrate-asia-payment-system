package exchange

import (
	"errors"
	"sync"
	"time"
)

// ExchangeRate represents the conversion rate between a fiat currency and a cryptocurrency
type ExchangeRate struct {
	FiatCurrency    string    `json:"fiat_currency"`
	CryptoCurrency  string    `json:"crypto_currency"`
	Rate            float64   `json:"rate"`
	LastUpdated     time.Time `json:"last_updated"`
	Source          string    `json:"source"`
}

// ExchangeRateService handles exchange rate operations
type ExchangeRateService struct {
	// In-memory cache of exchange rates
	rateCache     map[string]map[string]*ExchangeRate
	cacheMutex    sync.RWMutex
	updateInterval time.Duration
	
	// Dependencies would be injected here in a real implementation
	// exchangeAPIClients, etc.
}

// NewExchangeRateService creates a new instance of ExchangeRateService
func NewExchangeRateService() *ExchangeRateService {
	service := &ExchangeRateService{
		rateCache:      make(map[string]map[string]*ExchangeRate),
		updateInterval: 5 * time.Minute,
	}
	
	// In a real implementation, we would start a background goroutine to update rates periodically
	// go service.startRateUpdateLoop()
	
	return service
}

// GetExchangeRate retrieves the current exchange rate for a currency pair
func (s *ExchangeRateService) GetExchangeRate(fiatCurrency, cryptoCurrency string) (*ExchangeRate, error) {
	if fiatCurrency == "" || cryptoCurrency == "" {
		return nil, errors.New("both fiat currency and cryptocurrency must be specified")
	}
	
	// Check the cache first
	s.cacheMutex.RLock()
	fiatRates, exists := s.rateCache[fiatCurrency]
	if exists {
		rate, exists := fiatRates[cryptoCurrency]
		if exists {
			// Check if the rate is still fresh
			if time.Since(rate.LastUpdated) < s.updateInterval {
				s.cacheMutex.RUnlock()
				return rate, nil
			}
		}
	}
	s.cacheMutex.RUnlock()
	
	// Cache miss or stale data, fetch fresh rate
	rate, err := s.fetchExchangeRate(fiatCurrency, cryptoCurrency)
	if err != nil {
		return nil, err
	}
	
	// Update the cache
	s.cacheMutex.Lock()
	defer s.cacheMutex.Unlock()
	
	if _, exists := s.rateCache[fiatCurrency]; !exists {
		s.rateCache[fiatCurrency] = make(map[string]*ExchangeRate)
	}
	
	s.rateCache[fiatCurrency][cryptoCurrency] = rate
	
	return rate, nil
}

// GetAllExchangeRates retrieves all available exchange rates
func (s *ExchangeRateService) GetAllExchangeRates() ([]*ExchangeRate, error) {
	// In a real implementation, we would return all rates from the cache
	// For now, we'll return an empty slice
	return []*ExchangeRate{}, nil
}

// fetchExchangeRate retrieves the current exchange rate from external APIs
func (s *ExchangeRateService) fetchExchangeRate(fiatCurrency, cryptoCurrency string) (*ExchangeRate, error) {
	// In a real implementation, we would:
	// 1. Query multiple exchange rate APIs
	// 2. Aggregate and validate the results
	// 3. Apply any business logic (spreads, etc.)
	
	// For now, we'll return placeholder values
	now := time.Now()
	
	// Example rates for demonstration purposes
	rates := map[string]map[string]float64{
		"USD": {
			"BTC": 0.000025,
			"ETH": 0.00035,
			"USDT": 1.0,
		},
		"SGD": {
			"BTC": 0.000019,
			"ETH": 0.00026,
			"USDT": 0.75,
		},
		"MYR": {
			"BTC": 0.0000059,
			"ETH": 0.000082,
			"USDT": 0.23,
		},
		"THB": {
			"BTC": 0.00000078,
			"ETH": 0.000011,
			"USDT": 0.030,
		},
		"IDR": {
			"BTC": 0.00000000165,
			"ETH": 0.000000023,
			"USDT": 0.000064,
		},
		"VND": {
			"BTC": 0.00000000104,
			"ETH": 0.000000014,
			"USDT": 0.000041,
		},
		"BND": {
			"BTC": 0.000019,
			"ETH": 0.00026,
			"USDT": 0.75,
		},
		"KHR": {
			"BTC": 0.0000000025,
			"ETH": 0.000000035,
			"USDT": 0.00025,
		},
		"LAK": {
			"BTC": 0.00000000021,
			"ETH": 0.0000000029,
			"USDT": 0.000056,
		},
	}
	
	// Check if we have a rate for this pair
	if fiatRates, exists := rates[fiatCurrency]; exists {
		if rate, exists := fiatRates[cryptoCurrency]; exists {
			return &ExchangeRate{
				FiatCurrency:   fiatCurrency,
				CryptoCurrency: cryptoCurrency,
				Rate:           rate,
				LastUpdated:    now,
				Source:         "example_exchange",
			}, nil
		}
	}
	
	return nil, errors.New("exchange rate not available for the specified currency pair")
}

// startRateUpdateLoop periodically updates exchange rates in the background
func (s *ExchangeRateService) startRateUpdateLoop() {
	ticker := time.NewTicker(s.updateInterval)
	defer ticker.Stop()
	
	for range ticker.C {
		s.updateAllRates()
	}
}

// updateAllRates refreshes all exchange rates in the cache
func (s *ExchangeRateService) updateAllRates() {
	// In a real implementation, we would:
	// 1. Determine which currency pairs need updating
	// 2. Fetch fresh rates for those pairs
	// 3. Update the cache
	
	// This would be implemented in a production system
}

// ConvertAmount converts an amount from one currency to another
func (s *ExchangeRateService) ConvertAmount(amount float64, fromCurrency, toCurrency string) (float64, error) {
	if amount < 0 {
		return 0, errors.New("amount cannot be negative")
	}
	
	// Get the exchange rate
	rate, err := s.GetExchangeRate(fromCurrency, toCurrency)
	if err != nil {
		return 0, err
	}
	
	// Perform the conversion
	convertedAmount := amount * rate.Rate
	
	return convertedAmount, nil
}

// ApplySpread applies a percentage spread to an exchange rate
func (s *ExchangeRateService) ApplySpread(rate float64, spreadPercentage float64) float64 {
	if spreadPercentage < 0 {
		return rate
	}
	
	// Apply the spread
	// For buy rates (fiat to crypto), we increase the rate
	adjustedRate := rate * (1 + spreadPercentage/100)
	
	return adjustedRate
}
