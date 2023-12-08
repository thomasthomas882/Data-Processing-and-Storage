#include <iostream>
#include <unordered_map>
#include <stack>
#include <stdexcept>

class InMemoryDB {
private:
    std::unordered_map<std::string, int> data;  // main data store
    std::stack<std::unordered_map<std::string, int>> transactions;  // stack to store transactions

public:
    int get(const std::string& key) {
        return data.count(key) ? data[key] : -1;  // return -1 if key not found
    }

    void put(const std::string& key, int val) {
        if (transactions.empty()) {
            throw std::runtime_error("Transaction not in progress");
        }
        transactions.top()[key] = val;  // update transaction data
    }

    void begin_transaction() {
        transactions.push({});  // start a new transaction
    }

    void commit() {
        if (transactions.empty()) {
            throw std::runtime_error("No open transaction");
        }

        // merge changes from the transaction into the main data
        data.merge(transactions.top());
        transactions.pop();  // remove the committed transaction
    }

    void rollback() {
        if (transactions.empty()) {
            throw std::runtime_error("No ongoing transaction");
        }

        transactions.pop();  // remove the open transaction
    }
};

int main() {
    InMemoryDB inmemoryDB;

    // Example usage
    std::cout << inmemoryDB.get("A") << std::endl;  // should return -1

    // This will throw an error because a transaction is not in progress
    try {
        inmemoryDB.put("A", 5);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }

    inmemoryDB.begin_transaction();
    inmemoryDB.put("A", 5);
    std::cout << inmemoryDB.get("A") << std::endl;  // should return -1 (not committed yet)

    inmemoryDB.put("A", 6);
    inmemoryDB.commit();

    std::cout << inmemoryDB.get("A") << std::endl;  // should return 6

    // This will throw an error because there is no open transaction
    try {
        inmemoryDB.commit();
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }

    // This will throw an error because there is no ongoing transaction
    try {
        inmemoryDB.rollback();
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }

    std::cout << inmemoryDB.get("B") << std::endl;  // should return -1

    inmemoryDB.begin_transaction();
    inmemoryDB.put("B", 10);
    inmemoryDB.rollback();

    std::cout << inmemoryDB.get("B") << std::endl;  // should return -1 (changes rolled back)

    return 0;
}
