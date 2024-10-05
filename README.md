# Bitfinex CLI

This is a simple CLI program that allows you to interact with the [Bitfinex](https://www.bitfinex.com/) API.

## Features
This project is still work in progress, currently we offer the following functionalities:
- Place a new order
- Retrieve a ticker for a given symbol
- Cancel an order
- Retrieve order book for a given symbol

## How do I build ?
Before building the project, you will need to install the needded dependencies
### Dependencies
We're using cmake as our build tool. Install `Cmake 3.28` or a newer version:
```bash
sudo apt install cmake
```
Beside the build tool, we're using the following packages/libraries:
```bash
sudo apt install libboost-all-dev libssl-dev
```
We're using [nlohmann/json](https://github.com/nlohmann/json) library to parse JSON data, it's not available through the apt package manager, so you will need to install it from brew:
```bash
brew install nlohmann-json
```
We're also using [cpr](https://github.com/libcpr/cpr) as a C++ wrapper around Libcurl, however you don't need to worry about it since it's downloaded automatically in the cmake file through `FetchContent`

### Build
This project comes with a simple bash script that allows you to easily build or run the program, to build, assuming you're in the project root, run:
```bash
./trader.sh build
```
and to run the program:
```bash
./trader.sh run
```
### Demonstration
* Task: Let's say you want to place a limit order using symbol (tTESTBTC:TESTUSD) to buy 0.1 BTC for USD at a price higher than the best ask.

1- let's get ticker tTESTBTC:TESTUSD
```bash
./trader.sh run --ticker="tTESTBTC:TESTUSD"
```
output:
```
Price of the last trade: 62233
Price of last highest bid: 62129
Daily volume: 10.9283
```
The last highest bid is 62129 for 1 BTC, for 0.1 BTC that's `62129 * 0.1 = 6212.9`, let's offer a price much higher than `6212.9`, let's offer `8000`.

2- Now that we have some information about the symbol, let's place our order:
```bash
./trader.sh run --order --symbol="tTESTBTC:TESTUSD" --side=buy --type=exchange_limit --amount=0.1 --price=8000
```
output:
```
Placed a EXCHANGE LIMIT buy order for pair tTESTBTC:TESTUSD for the price of 8000 for 0.09 amount, order id: (order_id)
Would you like to change the order price ? (y,n)
```
Let's type y:
```
last trade price for pair tTESTBTC:TESTUSD for the amount 0.09 is 5600.97 to make sure the order does not fill offer a much higher/lower price
Please enter the new order price:
```
Let's enter 8500
```
Successfully changed order price to 8500
Would you like to cancel the order (order_id) ? (y,n)
```
Let's enter no

We successfully placed an order, got the chance to change the price and to cancel if we want to.

For more information about the supported features, run:
```bash
./trader.sh run help
```

## .env file
You will need a .env file at the project root with the following variables:
```
BASE_ENDPOINT=
API_KEY=
SECRET_KEY=
```
To learn how to create your API and SECRET KEY, visit [How to create and revoke a Bitfinex API Key](https://support.bitfinex.com/hc/en-us/articles/115003363429-How-to-create-and-revoke-a-Bitfinex-API-Key)

For BASE_ENDPOINT, see https://docs.bitfinex.com/docs/rest-general
