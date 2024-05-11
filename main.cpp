#include <iostream>
#include <deque>
#include <numeric>
#include <random>

// MarketData simulator generates pseudo-random prices
class MarketData {
private:
    std::default_random_engine generator;
    std::normal_distribution<double> distribution;
    double lastPrice;

public:
    MarketData(double mean, double stddev, double initialPrice) 
        : distribution(mean, stddev), lastPrice(initialPrice) {}

    double getNewPrice() {
        lastPrice += distribution(generator);  // Generate price change
        return lastPrice;
    }
};

// TradingBot makes decisions based on moving averages
class TradingBot {
private:
    std::deque<double> priceWindow;
    size_t windowSize;
    MarketData& marketData;
    double funds;
    int sharesOwned;

    double calculateMovingAverage() {
        if (priceWindow.size() < windowSize) return 0;
        return std::accumulate(priceWindow.begin(), priceWindow.end(), 0.0) / priceWindow.size();
    }

public:
    TradingBot(size_t windowSize, MarketData& data, double initialFunds)
        : windowSize(windowSize), marketData(data), funds(initialFunds), sharesOwned(0) {}

    void tick() {
        double currentPrice = marketData.getNewPrice();
        priceWindow.push_back(currentPrice);
        if (priceWindow.size() > windowSize) {
            priceWindow.pop_front();
        }

        double movingAverage = calculateMovingAverage();

        // Trading strategy: buy if current price is 10% below the moving average
        if (currentPrice < movingAverage * 0.9 && funds >= currentPrice) {
            int sharesToBuy = funds / currentPrice;
            sharesOwned += sharesToBuy;
            funds -= sharesToBuy * currentPrice;
            std::cout << "Buying " << sharesToBuy << " shares at " << currentPrice << std::endl;
        }
        // Sell if current price is 10% above the moving average
        else if (currentPrice > movingAverage * 1.1 && sharesOwned > 0) {
            funds += sharesOwned * currentPrice;
            std::cout << "Selling " << sharesOwned << " shares at " << currentPrice << std::endl;
            sharesOwned = 0;
        }
    }

    void printStatus() {
        std::cout << "Funds: $" << funds << ", Shares Owned: " << sharesOwned << std::endl;
    }
};

int main() {
    MarketData data(0.0, 1.0, 100.0);  // mean, stddev, initialPrice
    TradingBot bot(10, data, 1000.0);  // windowSize, MarketData, initialFunds

    for (int i = 0; i < 100; ++i) {
        bot.tick();
        bot.printStatus();
    }

    return 0;
}
