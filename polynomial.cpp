
#include "Polynomial.h"
#include <sstream>
#include <cmath>
#include <iostream>

using namespace std;

// Hidden Term structure (not in header)
struct Term {
    int coeff, exp;
    Term* next;
    Term(int c, int e) : coeff(c), exp(e), next(nullptr) {}
};

// Hidden structure to store polynomial data
struct PolynomialData {
    Term* head;
    PolynomialData() : head(nullptr) {}
};

// Static storage to hide implementation details
static PolynomialData storage[1000];
static Polynomial* objs[1000];
static int objCount = 0;

static int getIndex(const Polynomial* p) {
    for (int i = 0; i < objCount; i++) {
        if (objs[i] == p) return i;
    }
    objs[objCount] = const_cast<Polynomial*>(p);
    return objCount++;
}

static Term*& getHead(const Polynomial* p) {
    return storage[getIndex(p)].head;
}

// Constructor
Polynomial::Polynomial() {
    getHead(this) = nullptr;
}

// Destructor
Polynomial::~Polynomial() {
    Term* curr = getHead(this);
    while (curr) {
        Term* tmp = curr->next;
        delete curr;
        curr = tmp;
    }
}

// Insert a term into the polynomial
void Polynomial::insertTerm(int coefficient, int exponent) {
    if (coefficient == 0) return;

    Term*& head = getHead(this);
    Term* prev = nullptr;
    Term* curr = head;

    // Find position (keep sorted by descending exponent)
    while (curr && curr->exp > exponent) {
        prev = curr;
        curr = curr->next;
    }

    // If term with same exponent exists, combine
    if (curr && curr->exp == exponent) {
        curr->coeff += coefficient;
        if (curr->coeff == 0) {
            // Remove term if coefficient becomes zero
            if (prev)
                prev->next = curr->next;
            else
                head = curr->next;
            delete curr;
        }
        return;
    }

    // Insert new term
    Term* newTerm = new Term(coefficient, exponent);
    if (!prev) {
        // Insert at head
        newTerm->next = head;
        head = newTerm;
    } else {
        // Insert in middle or end
        newTerm->next = curr;
        prev->next = newTerm;
    }
}

// Return polynomial as a human-readable string
string Polynomial::toString() const {
    Term* curr = getHead(this);
    if (!curr) return "0";

    ostringstream out;
    bool first = true;

    while (curr) {
        int c = curr->coeff;
        int e = curr->exp;

        // Handle sign
        if (c > 0 && !first) out << " + ";
        if (c < 0) out << (first ? "-" : " - ");

        int absC = abs(c);

        // Format term based on exponent
        if (e == 0) {
            out << absC;
        } else if (e == 1) {
            if (absC != 1) out << absC;
            out << "x";
        } else {
            if (absC != 1) out << absC;
            out << "x^" << e;
        }

        first = false;
        curr = curr->next;
    }

    return out.str();
}

// Return a new polynomial that is the sum of this and other
Polynomial Polynomial::add(const Polynomial& other) const {
    Polynomial result;
    Term* a = getHead(this);
    Term* b = getHead(&other);

    while (a || b) {
        int coeff, exp;

        if (!b || (a && a->exp > b->exp)) {
            // Take from first polynomial
            coeff = a->coeff;
            exp = a->exp;
            a = a->next;
        } else if (!a || (b->exp > a->exp)) {
            // Take from second polynomial
            coeff = b->coeff;
            exp = b->exp;
            b = b->next;
        } else {
            // Same exponent, add coefficients
            coeff = a->coeff + b->coeff;
            exp = a->exp;
            a = a->next;
            b = b->next;
        }

        if (coeff != 0) {
            result.insertTerm(coeff, exp);
        }
    }

    return result;
}

// Return a new polynomial that is the product of this and other
Polynomial Polynomial::multiply(const Polynomial& other) const {
    Polynomial result;

    for (Term* a = getHead(this); a; a = a->next) {
        for (Term* b = getHead(&other); b; b = b->next) {
            int newCoeff = a->coeff * b->coeff;
            int newExp = a->exp + b->exp;
            result.insertTerm(newCoeff, newExp);
        }
    }

    return result;
}

// Return a new polynomial that is the derivative of this polynomial
Polynomial Polynomial::derivative() const {
    Polynomial result;

    for (Term* a = getHead(this); a; a = a->next) {
        if (a->exp > 0) {
            int newCoeff = a->coeff * a->exp;
            int newExp = a->exp - 1;
            result.insertTerm(newCoeff, newExp);
        }
    }

    return result;
}
