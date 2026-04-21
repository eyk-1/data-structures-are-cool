#pragma once
#include <vector>
#include <stdexcept>
#include "Product.hpp"
#include "AVLTree.hpp"
#include "OSTTree.hpp"

class Store {
private:
    // Price index: keyed by price → range queries
    AVLTree<int, Product> priceIndex;

    // Rating index: keyed by rating → top N by rating
    OSTTree<double, Product> ratingIndex;

    // Popularity index: keyed by popularity → top N by popularity
    OSTTree<int, Product> popularityIndex;

    // ---------------------------------------------------------------
    // Internal helpers
    // ---------------------------------------------------------------

    // Insert a product into all three indexes
    void indexProduct(const Product& p) {
        priceIndex.insert_31651(p.getPrice(),      p);
        ratingIndex.insert(p.getRating(),          p);
        popularityIndex.insert(p.getPopularity(),  p);
    }

    // Remove a product from all three indexes
    void deindexProduct(const Product& p) {
        priceIndex.remove_31651(p.getPrice());
        ratingIndex.remove(p.getRating());
        popularityIndex.remove(p.getPopularity());
    }

public:

    // ---------------------------------------------------------------
    // Product management
    // ---------------------------------------------------------------

    // Add a new product to the store
    void addProduct(const Product& p) {
        indexProduct(p);
    }

    // Remove a product from the store
    void removeProduct(const Product& p) {
        deindexProduct(p);
    }

    // Update a product: remove old entry, insert updated one
    // Pass the old product so we can remove by its old keys
    void updateProduct(const Product& oldP, const Product& newP) {
        deindexProduct(oldP);
        indexProduct(newP);
    }

    // ---------------------------------------------------------------
    // Queries
    // ---------------------------------------------------------------

    // Return all products with price in [low, high]
    // Uses AVL range walk: O(log n + k) where k = results returned
    std::vector<Product> priceRange(int low, int high) {
        if (low > high)
            throw std::invalid_argument("low must be <= high");

        std::vector<Product> results;
        priceIndex.rangeQuery(low, high, results);
        return results;
    }

    // Return the top N products by rating (highest first)
    // Uses OST reverse in-order traversal: O(log n + N)
    std::vector<Product> topNByRating(int n) {
        if (n <= 0)
            throw std::invalid_argument("n must be positive");

        return ratingIndex.getTopN(n);
    }

    // Return the top N products by popularity (highest first)
    // Uses OST reverse in-order traversal: O(log n + N)
    std::vector<Product> topNByPopularity(int n) {
        if (n <= 0)
            throw std::invalid_argument("n must be positive");

        return popularityIndex.getTopN(n);
    }

    // Return the product at rank k by rating (0 = lowest rated)
    // Useful for pagination: "give me rank 20 to 40"
    bool productAtRatingRank(int k, Product& out) {
        return ratingIndex.select(k, out);
    }

    // Return the product at rank k by popularity (0 = least popular)
    bool productAtPopularityRank(int k, Product& out) {
        return popularityIndex.select(k, out);
    }
};