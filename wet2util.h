// 234218 Data Structures 1.
// Semester: 2026A (Winter).
// Wet Exercise #2.
// 
// The following header file contains necessary types for your code.
// This file is READ ONLY: Submitting something else will not result
// in compiling your code with other than this file.
// 
// DO NOT use the preprocessors in your own code files.
// 

#ifndef WET2_UTIL_H_
#define WET2_UTIL_H_

#include <stdexcept>
#include <string>
#include <ostream>

// StatusType
enum struct StatusType {
	SUCCESS          = 0,
	ALLOCATION_ERROR = 1,
	INVALID_INPUT    = 2,
	FAILURE          = 3,
};

// output_t<T>
// The following class is used to support output with status code.
template<typename T>
class output_t {
private:
	// DO NOT access these private fields - may be enforced by verifier.
	const StatusType __status;
	const T __ans;

public:
	output_t() : __status(StatusType::SUCCESS), __ans(T()) { }
	output_t(StatusType status) : __status(status), __ans(T()) { }
	output_t(const T &ans) : __status(StatusType::SUCCESS), __ans(ans) { }
	
	StatusType status() { return __status; }
	T ans() { return __ans; }
};


// NenAbility
// The following class represents a Nen ability.
class NenAbility {
private:
    int types[6];
    // index mapping:
    // 0 = Enhancer
    // 1 = Emitter
    // 2 = Transmuter
    // 3 = Conjurer
    // 4 = Manipulator
    // 5 = Specialist

public:
    
    NenAbility() {
        for (int i = 0; i < 6; i++) types[i] = 0;
    }

    virtual ~NenAbility() = default;

    explicit NenAbility(const std::string& type) {
        for (int i = 0; i < 6; i++) types[i] = 0;

        if      (type == "Enhancer")        types[0] = 1;
        else if (type == "Emitter")         types[1] = 1;
        else if (type == "Transmuter")      types[2] = 1;
        else if (type == "Conjurer")        types[3] = 1;
        else if (type == "Manipulator")     types[4] = 1;
        else if (type == "Specialist")      types[5] = 1;
        else {
            // invalid: mark all entries as -1
            for (int i = 0; i < 6; i++) types[i] = -1;
        }
    }

    NenAbility(const NenAbility &other) {
        for (int i = 0; i < 6; i++) types[i] = other.types[i];
    }

    NenAbility& operator=(const NenAbility &other) {
        if (this != &other) {
            for (int i = 0; i < 6; i++)
                types[i] = other.types[i];
        }
        return *this;
    }


    static NenAbility zero() { return NenAbility(); }

    static NenAbility invalid() {
        NenAbility n;
        for (int i = 0; i < 6; i++) n.types[i] = -1;
        return n;
    }

    bool isValid() const {
        for (int i = 0; i < 6; i++)
            if (types[i] < 0) return false;
        return true;
    }

    // Effective ability 
    // This the strength metric for force_join function
    int getEffectiveNenAbility() const { // L2 norm
        int sum = 0;
        for (int i = 0; i < 6; i++) sum += (types[i] * types[i]);
        return sum;
    }

    // Comparetors for squad_duel function
    /*
    Nen abilities form a circular, non-transitive matchup pattern. Enhancers overpower
    Transmuters, Emitters disrupt Enhancers, Transmuters outmaneuver Emitters, Conjurers
    restrict Transmuters, and Manipulators override Conjurers but struggle against Emitters.
    This creates a closed loop of advantages, with each type strong against one and vulnerable
    to another. Specialists gain a soft advantage over all categories.
    
    This matrix encodes that matchup circle:
    */
   static const int (&getNenMatrix())[6][6] {
        static const int matrix[6][6] = {
            // E   Em  Tr  Co  Ma  Sp
            {  0, +1, +1, -1, -1, -1 }, // Enhancer
            { -1,  0, +1, +1, -1, -1 }, // Emitter
            { -1, -1,  0, +1, +1, -1 }, // Transmuter
            { +1, -1, -1,  0, +1, -1 }, // Conjurer
            { +1, +1, -1, -1,  0, -1 }, // Manipulator
            { +1, +1, +1, +1, +1,  0 }  // Specialist  
        };
        return matrix;
    }

    static int compareNenTypes(const int A[6], const int B[6]) {
        int score = 0;
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 6; j++) {
                score += A[i] * B[j] * getNenMatrix()[i][j];
            }
        }
        return score;
    }
    bool operator==(const NenAbility &other) const {
        return compareNenTypes(this->types, other.types) == 0;
    }

    bool operator<(const NenAbility &other) const {
        return compareNenTypes(this->types, other.types) < 0;
    }

    bool operator>(const NenAbility &other) const {
        return compareNenTypes(this->types, other.types) > 0;
    }
    
    // Arithmetics:
    NenAbility& operator+=(const NenAbility &other) {
        for (int i = 0; i < 6; i++)
            types[i] += other.types[i];
        return *this;
    }

    NenAbility& operator-=(const NenAbility &other) {
        for (int i = 0; i < 6; i++)
            types[i] -= other.types[i];
        return *this;
    }

    friend NenAbility operator+(NenAbility a, const NenAbility &b) {
        a += b;
        return a;
    }

    friend NenAbility operator-(NenAbility a, const NenAbility &b) {
        a -= b;
        return a;
    }

    NenAbility inv() const {
        NenAbility res;
        for (int i = 0; i < 6; i++)
            res.types[i] = -types[i];
        return res;
    }
    NenAbility operator-() const {
        return this->inv();
    }

    friend std::ostream& operator<<(std::ostream &out, const NenAbility &obj) {
        if (!obj.isValid()) {
            out << "Invalid NenAbility";
            return out;
        }

        static const char* names[6] = {
            "Enhancer",
            "Emitter",
            "Transmuter",
            "Conjurer",
            "Manipulator",
            "Specialist"
        };

        for (int i = 0; i < 6; i++) {
            if (i > 0) out << ", ";
            out << names[i] << ":" << obj.types[i];
        }

        return out;
    }
};



#endif // WET2_UTIL_H_
