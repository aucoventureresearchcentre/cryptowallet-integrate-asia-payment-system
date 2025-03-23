# Asian Cryptocurrency Payment System - API Reference

## Overview

This API reference provides detailed documentation for the Asian Cryptocurrency Payment System API. The API allows merchants to create and manage cryptocurrency payments, check transaction statuses, retrieve exchange rates, and access regulatory information for supported Asian countries.

## Base URL

```
https://api.asiancryptopay.com/v1
```

## Authentication

All API requests require authentication using your API key and request signing.

### Headers

| Header | Description |
|--------|-------------|
| `X-API-Key` | Your API key |
| `X-Timestamp` | Current Unix timestamp in milliseconds |
| `X-Nonce` | Random string to prevent replay attacks |
| `X-Signature` | HMAC signature of the request |

### Signature Generation

The signature is an HMAC-SHA256 hash of the following components:

- HTTP method (e.g., `GET`, `POST`)
- Endpoint path (e.g., `/payments`)
- Timestamp
- Nonce
- Request body (for POST requests)

Example signature generation in JavaScript:

```javascript
const crypto = require('crypto');

function generateSignature(method, endpoint, timestamp, nonce, body, apiSecret) {
  const payload = method + endpoint + timestamp + nonce + (body ? JSON.stringify(body) : '');
  return crypto.createHmac('sha256', apiSecret).update(payload).digest('hex');
}
```

## Endpoints

### Payments

#### Create Payment

Creates a new cryptocurrency payment.

```
POST /payments
```

**Request Body:**

```json
{
  "amount": 100.00,
  "currency": "SGD",
  "crypto_currency": "BTC",
  "merchant_id": "your_merchant_id",
  "country_code": "SG",
  "order_id": "ORD-123456",
  "description": "Product purchase",
  "callback_url": "https://your-website.com/payment-callback",
  "success_url": "https://your-website.com/payment-success",
  "cancel_url": "https://your-website.com/payment-cancel",
  "metadata": {
    "customer_id": "CUST-123",
    "product_id": "PROD-456"
  },
  "test_mode": false
}
```

**Response:**

```json
{
  "transaction_id": "TRX-789012",
  "payment_address": "bc1q9h6mqc7h5jqz8q5u4nqzz5xw9svmcydk7r8xr5",
  "payment_url": "https://pay.asiancryptopay.com/p/TRX-789012",
  "amount": 100.00,
  "currency": "SGD",
  "crypto_amount": 0.00234567,
  "crypto_currency": "BTC",
  "exchange_rate": 42631.25,
  "status": "pending",
  "created_at": "2025-03-22T16:15:23Z",
  "expires_at": "2025-03-22T16:45:23Z"
}
```

#### Get Payment Status

Retrieves the status of a payment.

```
GET /payments/{transaction_id}
```

**Response:**

```json
{
  "transaction_id": "TRX-789012",
  "payment_address": "bc1q9h6mqc7h5jqz8q5u4nqzz5xw9svmcydk7r8xr5",
  "amount": 100.00,
  "currency": "SGD",
  "crypto_amount": 0.00234567,
  "crypto_currency": "BTC",
  "status": "completed",
  "blockchain_txid": "7a9f8e7d6c5b4a3b2c1d0e9f8a7b6c5d4e3f2a1b0c9d8e7f6a5b4c3d2e1f0a9",
  "confirmations": 3,
  "created_at": "2025-03-22T16:15:23Z",
  "updated_at": "2025-03-22T16:25:45Z",
  "completed_at": "2025-03-22T16:25:45Z"
}
```

#### List Payments

Retrieves a list of payments.

```
GET /payments
```

**Query Parameters:**

| Parameter | Description |
|-----------|-------------|
| `status` | Filter by status (pending, completed, failed, expired) |
| `start_date` | Filter by start date (ISO 8601 format) |
| `end_date` | Filter by end date (ISO 8601 format) |
| `limit` | Number of results per page (default: 20, max: 100) |
| `offset` | Pagination offset (default: 0) |

**Response:**

```json
{
  "total": 42,
  "limit": 20,
  "offset": 0,
  "payments": [
    {
      "transaction_id": "TRX-789012",
      "amount": 100.00,
      "currency": "SGD",
      "crypto_amount": 0.00234567,
      "crypto_currency": "BTC",
      "status": "completed",
      "created_at": "2025-03-22T16:15:23Z",
      "completed_at": "2025-03-22T16:25:45Z"
    },
    // More payments...
  ]
}
```

### Exchange Rates

#### Get Exchange Rate

Retrieves the current exchange rate between a fiat currency and a cryptocurrency.

```
GET /exchange-rates
```

**Query Parameters:**

| Parameter | Description |
|-----------|-------------|
| `fiat` | Fiat currency code (e.g., SGD, MYR, IDR) |
| `crypto` | Cryptocurrency code (e.g., BTC, ETH, USDT) |

**Response:**

```json
{
  "fiat": "SGD",
  "crypto": "BTC",
  "rate": 42631.25,
  "timestamp": "2025-03-22T16:30:00Z"
}
```

#### Get All Exchange Rates

Retrieves all available exchange rates.

```
GET /exchange-rates/all
```

**Response:**

```json
{
  "timestamp": "2025-03-22T16:30:00Z",
  "rates": {
    "SGD": {
      "BTC": 42631.25,
      "ETH": 2845.67,
      "USDT": 1.35
    },
    "MYR": {
      "BTC": 42950.75,
      "ETH": 2867.89,
      "USDT": 1.36
    },
    // More currencies...
  }
}
```

### Cryptocurrencies

#### Get Supported Cryptocurrencies

Retrieves the list of supported cryptocurrencies for a specific country.

```
GET /cryptocurrencies
```

**Query Parameters:**

| Parameter | Description |
|-----------|-------------|
| `country` | ISO country code (e.g., SG, MY, ID) |

**Response:**

```json
{
  "country": "SG",
  "cryptocurrencies": [
    {
      "code": "BTC",
      "name": "Bitcoin",
      "logo_url": "https://assets.asiancryptopay.com/crypto/btc.png",
      "min_amount": 0.0001,
      "max_amount": 10.0,
      "confirmations_required": 2
    },
    {
      "code": "ETH",
      "name": "Ethereum",
      "logo_url": "https://assets.asiancryptopay.com/crypto/eth.png",
      "min_amount": 0.01,
      "max_amount": 100.0,
      "confirmations_required": 12
    },
    // More cryptocurrencies...
  ]
}
```

### Regulatory Information

#### Get Regulatory Information

Retrieves regulatory information for a specific country.

```
GET /regulatory-info
```

**Query Parameters:**

| Parameter | Description |
|-----------|-------------|
| `country` | ISO country code (e.g., SG, MY, ID) |

**Response:**

```json
{
  "country": "SG",
  "country_name": "Singapore",
  "regulatory_authorities": [
    {
      "name": "Monetary Authority of Singapore (MAS)",
      "website": "https://www.mas.gov.sg"
    }
  ],
  "licensing_requirements": [
    "Payment Services Act (PSA) license for Digital Payment Token services"
  ],
  "transaction_limits": {
    "daily": 20000.00,
    "monthly": 100000.00
  },
  "kyc_requirements": {
    "individual": [
      "Full Name",
      "Date of Birth",
      "Nationality",
      "ID Document",
      "Residential Address",
      "Contact Information"
    ],
    "business": [
      "Business Name",
      "Registration Number",
      "Business Address",
      "Director Information",
      "Shareholder Information"
    ]
  },
  "reporting_requirements": [
    "Suspicious Transaction Reports (STRs)",
    "Cash Transaction Reports (CTRs) for transactions above SGD 20,000"
  ],
  "travel_rule": {
    "required": true,
    "threshold": 1000.00
  }
}
```

### Merchant Information

#### Get Merchant Profile

Retrieves the merchant's profile information.

```
GET /merchant
```

**Response:**

```json
{
  "merchant_id": "your_merchant_id",
  "name": "Your Business Name",
  "email": "contact@your-business.com",
  "country": "SG",
  "registration_date": "2025-01-15T08:30:00Z",
  "status": "active",
  "supported_countries": ["SG", "MY", "ID", "TH"],
  "supported_cryptocurrencies": ["BTC", "ETH", "USDT"],
  "compliance_status": {
    "SG": "approved",
    "MY": "approved",
    "ID": "pending",
    "TH": "approved"
  }
}
```

#### Update Merchant Profile

Updates the merchant's profile information.

```
PUT /merchant
```

**Request Body:**

```json
{
  "name": "Your Updated Business Name",
  "email": "new-contact@your-business.com",
  "callback_url": "https://your-website.com/updated-callback",
  "webhook_secret": "your_new_webhook_secret"
}
```

**Response:**

```json
{
  "merchant_id": "your_merchant_id",
  "name": "Your Updated Business Name",
  "email": "new-contact@your-business.com",
  "country": "SG",
  "registration_date": "2025-01-15T08:30:00Z",
  "status": "active",
  "callback_url": "https://your-website.com/updated-callback",
  "webhook_secret_updated": true
}
```

### Webhooks

The system can send webhook notifications for payment events to your callback URL.

#### Webhook Payload

```json
{
  "event": "payment.completed",
  "timestamp": "2025-03-22T16:25:45Z",
  "data": {
    "transaction_id": "TRX-789012",
    "merchant_id": "your_merchant_id",
    "amount": 100.00,
    "currency": "SGD",
    "crypto_amount": 0.00234567,
    "crypto_currency": "BTC",
    "status": "completed",
    "blockchain_txid": "7a9f8e7d6c5b4a3b2c1d0e9f8a7b6c5d4e3f2a1b0c9d8e7f6a5b4c3d2e1f0a9",
    "confirmations": 3,
    "created_at": "2025-03-22T16:15:23Z",
    "completed_at": "2025-03-22T16:25:45Z",
    "metadata": {
      "customer_id": "CUST-123",
      "product_id": "PROD-456"
    }
  }
}
```

#### Webhook Signature

Webhook requests include a signature header for verification:

| Header | Description |
|--------|-------------|
| `X-Webhook-Signature` | HMAC-SHA256 signature of the webhook payload |

Verify the signature using your webhook secret:

```javascript
const crypto = require('crypto');

function verifyWebhookSignature(payload, signature, secret) {
  const expectedSignature = crypto
    .createHmac('sha256', secret)
    .update(JSON.stringify(payload))
    .digest('hex');
  
  return crypto.timingSafeEqual(
    Buffer.from(signature),
    Buffer.from(expectedSignature)
  );
}
```

#### Webhook Events

| Event | Description |
|-------|-------------|
| `payment.created` | A new payment has been created |
| `payment.pending` | Payment has been detected but not confirmed |
| `payment.completed` | Payment has been completed successfully |
| `payment.failed` | Payment has failed |
| `payment.expired` | Payment has expired |
| `payment.refunded` | Payment has been refunded |

## Error Handling

The API uses standard HTTP status codes and returns error details in the response body.

### Error Response Format

```json
{
  "error": {
    "code": "invalid_request",
    "message": "Invalid payment amount",
    "details": "Payment amount must be greater than zero"
  }
}
```

### Common Error Codes

| Code | Description |
|------|-------------|
| `authentication_error` | Invalid API key or signature |
| `invalid_request` | Invalid request parameters |
| `resource_not_found` | Requested resource not found |
| `rate_limit_exceeded` | Too many requests |
| `internal_error` | Internal server error |
| `compliance_error` | Regulatory compliance issue |

## Rate Limits

The API enforces rate limits to prevent abuse:

| Endpoint | Rate Limit |
|----------|------------|
| `/payments` (POST) | 60 requests per minute |
| `/payments` (GET) | 120 requests per minute |
| `/exchange-rates` | 300 requests per minute |
| Other endpoints | 60 requests per minute |

Rate limit headers are included in API responses:

| Header | Description |
|--------|-------------|
| `X-RateLimit-Limit` | Maximum requests per minute |
| `X-RateLimit-Remaining` | Remaining requests in the current window |
| `X-RateLimit-Reset` | Time when the rate limit resets (Unix timestamp) |

## Pagination

List endpoints support pagination using `limit` and `offset` parameters:

```
GET /payments?limit=20&offset=40
```

Pagination information is included in the response:

```json
{
  "total": 142,
  "limit": 20,
  "offset": 40,
  "has_more": true,
  "payments": [
    // 20 payment objects
  ]
}
```

## SDK Integration

For easier integration, use our SDKs which handle authentication, request signing, and error handling:

- [JavaScript SDK](../installation/installation_guide.md#web-integration)
- [Java/Android SDK](../installation/installation_guide.md#pos-terminal-integration)
- [C++/Qt SDK](../installation/installation_guide.md#kiosk-integration)

## Testing

Use test mode to integrate and test your implementation without processing real transactions:

1. Set `test_mode: true` in your API requests
2. Use test API credentials from your merchant dashboard

Test mode provides simulated responses with the same structure as production mode.

## Support

For API support:

- Documentation: [docs.asiancryptopay.com](https://docs.asiancryptopay.com)
- Support Email: api-support@asiancryptopay.com
- Developer Forum: [forum.asiancryptopay.com](https://forum.asiancryptopay.com)
