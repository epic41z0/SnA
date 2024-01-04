#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <chrono>
#include <algorithm>

class BankAccount {
    std::string accountNumber;
    float balance;

public:
    BankAccount() {}

    BankAccount(std::string accountNumber, float balance = 0)
        : accountNumber(accountNumber), balance(balance) {}

    std::string getAccountNumber() { return this->accountNumber; }
};

//INTERFACE - gränssnitt "standard"
class IAccountStorage {
public:
    virtual void addAccount(BankAccount account) = 0;
    virtual BankAccount *findAccount(std::string accountNumber) = 0;
};

class Bank {
private:
    IAccountStorage *accountStorage;

public:
    Bank(IAccountStorage *storage) : accountStorage(storage) {}

    bool addAccount(std::string accountNumber) {
        accountStorage->addAccount(BankAccount(accountNumber));
        return true;
    }

    BankAccount *getAccount(std::string accountNumber) {
        return accountStorage->findAccount(accountNumber);
    }
};

class MapAccountStorage : public IAccountStorage {
    std::map<std::string, BankAccount> accounts;

public:
    void addAccount(BankAccount account) override {
        accounts[account.getAccountNumber()] = account;
    }

    BankAccount *findAccount(std::string accountNumber) override {
        auto it = accounts.find(accountNumber);
        if (it != accounts.end()) {
            return &(it->second);
        }
        return nullptr;
    }
};

class VectorAccountStorage : public IAccountStorage {
    std::vector<BankAccount> accounts;

public:
    void addAccount(BankAccount account) override {
        accounts.push_back(account);
    }

    BankAccount *findAccount(std::string accountNumber) override {
        // Binary search for account in sorted vector
        int low = 0, high = accounts.size() - 1;
        while (low <= high) {
            int mid = low + (high - low) / 2;
            if (accounts[mid].getAccountNumber() == accountNumber) {
                return &accounts[mid];
            } else if (accounts[mid].getAccountNumber() < accountNumber) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
        return nullptr;
    }
};

std::string padAccountNumber(const std::string &accountNumber) {
    std::string padded = accountNumber;
    while (padded.length() < 10) {
        padded = "0" + padded;
    }
    return padded;
}

int main() {
    VectorAccountStorage storage;
    Bank bank(&storage);

    const int AntalAccounts = 1000000;

    std::vector<int> accountNumbers;
    for (int i = 0; i < AntalAccounts; ++i) {
        accountNumbers.push_back(i);
    }
    std::random_device rd;
    std::default_random_engine g(rd());

    std::shuffle(accountNumbers.begin(), accountNumbers.end(), g);

    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < AntalAccounts; i++) {
        std::string accountNumber = std::to_string(accountNumbers[i]);
        accountNumber = padAccountNumber(accountNumber);
        bank.addAccount(accountNumber);
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "Adding accounts took: "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count()
              << " nanoseconds" << std::endl;

    std::string sFirst = padAccountNumber("0");
    std::string sLast = padAccountNumber(std::to_string(AntalAccounts - 1));
    std::string sNotFound = "notfound";

    startTime = std::chrono::high_resolution_clock::now();
    BankAccount *p = bank.getAccount(sFirst);
    endTime = std::chrono::high_resolution_clock::now();
    if (p != nullptr) {
        std::cout << p->getAccountNumber() << " took: "
                  << std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count()
                  << " nanoseconds" << std::endl;
    } else {
        std::cout << "First account not found!" << std::endl;
    }

    startTime = std::chrono::high_resolution_clock::now();
    p = bank.getAccount(sLast);
    endTime = std::chrono::high_resolution_clock::now();
    if (p != nullptr) {
        std::cout << p->getAccountNumber() << " took: "
                  << std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count()
                  << " nanoseconds" << std::endl;
    } else {
        std::cout << "Last account not found!" << std::endl;
    }

    startTime = std::chrono::high_resolution_clock::now();
    p = bank.getAccount(sNotFound);
    endTime = std::chrono::high_resolution_clock::now();
    if (p != nullptr) {
        std::cout << p->getAccountNumber() << " took: "
                  << std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count()
                  << " nanoseconds" << std::endl;
    } else {
        std::cout << "Account 'notfound' not found!" << std::endl;
    }

    return 0;
}