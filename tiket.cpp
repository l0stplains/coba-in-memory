#include <bits/stdc++.h>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <utility>

using namespace std;

void sleep(int ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
// value is a pair of currecy exchange rate with its TTL
unordered_map<string, pair<float, pair<chrono::steady_clock::time_point, int>>>
    in_memory_currency_store;

int totalLocalCacheHit = 0;

int getRandomWithin(int start, int range) {

  int randomValue = std::rand() % range + start;

  return randomValue;
}

int getRandomNumberOneToHundred() { return getRandomWithin(1, 100); }

float mockRedisFetch(string exchangeCode) {
  if (in_memory_currency_store.count(exchangeCode) != 0) {
    pair<float, pair<chrono::steady_clock::time_point, int>> storedData =
        in_memory_currency_store[exchangeCode];

    int TTL = storedData.second.second;

    auto start = storedData.second.first;
    auto now = std::chrono::steady_clock::now();

    auto elapsed =
        chrono::duration_cast<chrono::milliseconds>(now - start).count();

    // more than TTL, fall through to request new data
    if (elapsed > TTL) {
      in_memory_currency_store.erase(exchangeCode);
    } else {
      totalLocalCacheHit++;
      return storedData.first;
    }
  }

  // simulate loading 30ms
  sleep(30);

  float randomExchangeRate = (float)getRandomNumberOneToHundred();
  auto now = chrono::steady_clock::now();
  int TTL = 100;

  in_memory_currency_store[exchangeCode] =
      make_pair(randomExchangeRate, make_pair(now, TTL));

  return randomExchangeRate;

  // harusnya ada if exchangeCode not found, tapi di simulasi ini anggap semua
  // ada :v
}

void simulateRequests() {

  cout << "Start" << endl;

  const int REQUEST_NUM = 100;
  vector<string> exchangeCodeList = {"IDR-SGD", "SGD-IDR", "USD-SGD",
                                     "SGD-USD", "IDR-USD", "USD-IDR"};

  for (int i = 0; i < REQUEST_NUM; i++) {
    string randomCode =
        exchangeCodeList[getRandomWithin(0, (int)exchangeCodeList.size() - 1)];

    float exchangeRate = mockRedisFetch(randomCode);
  }

  cout << "Cache Hit: " << (float)totalLocalCacheHit / REQUEST_NUM << endl;

  cout << "Total request: " << REQUEST_NUM << endl;
  cout << "Total saved request: " << totalLocalCacheHit << " (saving "
       << (totalLocalCacheHit * 30) / 1000
       << " seconds for 30ms per redis call)" << endl;
}

int main() { simulateRequests(); }
