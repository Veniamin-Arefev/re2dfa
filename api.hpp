#include <string>
#include <set>


class Alphabet { // Contains alphabet symbols (chars)
    void *impl;

public:
    Alphabet(const std::string &raw_string);
    // Initialize with string containing all symbols of alphabet. Duplicate characters in the sting are ignored.

    Alphabet(const char *raw_string); // Same..
    Alphabet(const std::set<char> &charset); // Initialize with set of characters.

    std::string to_string() const; // Represent as a string containing all alphabet symbols.

    bool has_char(char c) const; // Check if c is in this alphabet.
    size_t size() const; // Number of symbols in the alphabet.
    bool insert(char c);
    // Add c to the alphabet. Returns false, if c was already there or symbol c can't be used in an alphabet.

    bool operator==(const Alphabet &other) const; // Check if this and other alphabets are identical.
    bool operator!=(const Alphabet &other) const; // Check if this and other alphabets are not identical.

    Alphabet(Alphabet &&a); // Alphabet has efficient copy and move constructors and operators.
    Alphabet(const Alphabet &a);

    Alphabet &operator=(const Alphabet &other);

    Alphabet &operator=(Alphabet &&other);

    ~Alphabet();

    const char *begin() const; // constant iterators
    const char *end() const;
};

class DFA { // Deterministic finite automaton
    void *impl;

public:
    DFA(const Alphabet &alphabet); // Constructor accepts Alphabet to determine automaton's alphabet.

    size_t size() const; // Number of states in this automaton
    bool is_empty() const; // Checks if this automaton consists of one non-final state.

    bool create_state(const std::string &name, bool is_final = false);
    // Add state with a certain name. is_final determines if state is final.
    // If name was already in this automaton, then only finality of the existing state is adjusted,
    // ... and the function returns false.

    bool has_state(const std::string &name) const;
    // Check if a state with this name exists in this automaton. O(1)

    std::set<std::string> get_states() const; // Get all the state names in this automaton.
    bool delete_state(const std::string &name);
    // Deletes a state with this name if it exists, or returns false if it doesn't.
    // All transitions to and from this state are also removed.

    std::string get_initial_state() const; // Get the name of the initial state.
    // !!!!! The initial state must already be set at this point. Otherwise the behavior will be undefined !!!!!

    bool is_initial(const std::string &name) const;
    // Check if the state with this name exists and is initial.

    bool set_initial(const std::string &name);
    // Makes state with this name initial. If this state doesn't exist, returns false and does nothing else.

    std::set<std::string> get_final_states() const; // Names of all final states in this automaton.
    bool is_final(const std::string &name) const; // Check if the state with this name exists and is final.
    bool make_final(const std::string &name);
    // Makes state with this name final. If this state doesn't exist, returns false and does nothing else.

    bool has_trans(const std::string &name, char trans_symbol) const;
    // Returns true if state with this name exists, trans_symbol is in the alphabet...
    // ...and transition from this state by trans_symbol exists in this automaton.

    std::string get_trans(const std::string &name, char trans_symbol) const;
    // Returns the name of the state, where the transition from the state with the give name...
    // ... by trans_symbol leads.
    // !!!!! has_trans(name, trans_symbol) must be true !!!!!!!!!!! Otherwise throws error and terminates !

    bool delete_trans(const std::string &from, char by);
    // Deletes transition from state from by symbol by
    // If from or to don't exist in the automaton, or by isn't in the alphabet, returns false and does nothing else.

    bool set_trans(const std::string &from, char by, const std::string &to);
    // Creates transition from state from to state to by symbol by.
    // If from or to don't exist in the automaton, or by isn't in the alphabet, returns false and does nothing else.
    // If there was already a transition from from by by, it is overwritten.

    Alphabet get_alphabet() const; // Get automaton's alphabet.

    std::string to_string() const;

    // Represent automaton in text format. Alphabet, all states (starting with initial) and then all transitions.
    // Automaton must be correct. (Have states, have an initial state)
    static DFA from_string(const std::string &); // Get automaton from text representation.

    DFA(DFA &&a); // Efficient copy and move constructors and operators.
    DFA(const DFA &a); // The whole structure is copied (states, transitions..)
    DFA &operator=(const DFA &other);

    DFA &operator=(DFA &&other);

    ~DFA();

};
