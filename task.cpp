#include "api.hpp"
#include <string>
#include <iostream>
#include <cctype>
#include <vector>
#include <set>
#include <algorithm>

std::vector<int> char_index, operator_index, operator_priority;
std::vector<std::set<int>> followpos;

class Tree {
public:
//    just element, from alphabet, operator or \n - means epsilon
    char elem;
    int index;
    Tree *left = nullptr;
    Tree *right = nullptr;
    bool nullable = NULL;
    std::set<int> firstpos, lastpos;

    Tree(char elem, int index, Tree *left, Tree *right) : elem(elem), index(index), left(left), right(right) {}

    ~Tree() {
        delete left;
        delete right;
    }

    void calculate_nullable_firstpos_lastpos() {
        if (left != nullptr) {
            left->calculate_nullable_firstpos_lastpos();
        }
        if (right != nullptr) {
            right->calculate_nullable_firstpos_lastpos();
        }
        if (elem == '\n') {
            nullable = true;
            // empty firstpos, lastpos
        } else if (std::isalnum(elem) || elem == '#') {
            nullable = elem == '#';
            firstpos.insert(index);
            lastpos.insert(index);
        } else if (elem == '|') {
            nullable = left->nullable || right->nullable;
            firstpos = left->firstpos;
            firstpos.insert(right->firstpos.begin(), right->firstpos.end());
            lastpos = left->lastpos;
            lastpos.insert(right->lastpos.begin(), right->lastpos.end());
        } else if (elem == '.') {
            nullable = left->nullable && right->nullable;
            firstpos = left->firstpos;
            if (left->nullable) {
                firstpos.insert(right->firstpos.begin(), right->firstpos.end());
            }
            lastpos = right->lastpos;
            if (right->nullable) {
                lastpos.insert(left->lastpos.begin(), left->lastpos.end());
            }
        } else if (elem == '*') {
            nullable = true;
            firstpos = left->firstpos;
            lastpos = left->lastpos;
        }
    };

    void calculate_followpos() {
        if (left != nullptr) {
            left->calculate_followpos();
        }
        if (right != nullptr) {
            right->calculate_followpos();
        }
        if (elem == '*') {
            for (auto &item: left->lastpos) {
                followpos[item].insert(left->firstpos.begin(), left->firstpos.end());
            }
        } else if (elem == '.') {
            for (auto &item: left->lastpos) {
                followpos[item].insert(right->firstpos.begin(), right->firstpos.end());
            }
        }
    }

};

Tree *rec_tree(const std::string &s, int start, int end) {
//    minify aka remove ()
    int offset = 0;
    while (s[start + offset] == '(' && s[end - offset] == ')') {
        offset += 1;
        int count = 0;
        bool is_end = false;
        for (char &elem: s.substr(start + offset, end - start + 1 - 2 * offset)) {
            if (elem == '(') {
                count++;
            } else if (elem == ')') {
                count--;
                if (count < 0) {
                    is_end= true;
                    offset--;
                    break;
                }
            }
        }
        if (is_end) {
            break;
        }
    }
    start += offset;
    end -= offset;

//    std::cout << s.substr(start, end - start + 1) << ' ' << start << ' ' << end << std::endl;

//    get the least priority operator
    int min_index = -1;
    for (int i = 0; i < operator_index.size(); i++) {
        if (operator_index[i] < start) {
            continue;
        }
        if (operator_index[i] > end) {
            break;
        }
        if (operator_priority[i] != 0) {
            if (min_index == -1 || operator_priority[i] <= operator_priority[min_index]) {
                min_index = i;
            }
        }
    }
//    std::cout << min_index << std::endl;
    Tree *tree = nullptr;
    if (min_index != -1) {
        char cur_operator = s[operator_index[min_index]];
        if (cur_operator == '|' || cur_operator == '.' || cur_operator == '*') {
            tree = new Tree(cur_operator, 0, nullptr, nullptr);
            tree->left = rec_tree(s, start, operator_index[min_index] - 1);
            if (cur_operator == '|' || cur_operator == '.') {
                tree->right = rec_tree(s, operator_index[min_index] + 1, end);
            }
        } else {
            std::cout << "ERROR!!!!" << std::endl;
        }
    } else {
        if (start == end) {
            int index_in_char_index = 0;
            while (char_index[index_in_char_index] != start) {
                index_in_char_index++;
            }
            tree = new Tree(s[start], index_in_char_index, nullptr, nullptr);
        } else if (start > end) {
            tree = new Tree('\n', -1, nullptr, nullptr);
        } else {
            std::cout << "ERROR!!!!" << std::endl;
        }
    }

    return tree;
}


Tree *create_tree(const std::string &s) {
    char_index = std::vector<int>();
    operator_index = std::vector<int>();

    for (int i = 0; i < s.size(); i++) {
        if (isalnum(s[i]) || s[i] == '#') {
            char_index.push_back(i);
        } else {
            operator_index.push_back(i);
        }
    }

//    for (auto &elem: operator_index) {
//        std::cout << s[elem] << "  ";
//    }
//    std::cout << std::endl;

//    for (auto &elem: operator_index) {
//        std::cout << elem << "  ";
//    }
//    std::cout << std::endl;

    operator_priority = std::vector<int>(operator_index.size());

    int cur_priority = 0;
    for (int i = 0; i < operator_index.size(); i++) {
        if (s[operator_index[i]] == '(') {
            cur_priority += 4;
            operator_priority[i] = 0;
        } else if (s[operator_index[i]] == ')') {
            cur_priority -= 4;
            operator_priority[i] = 0;
        } else if (s[operator_index[i]] == '|') {
            operator_priority[i] = 1 + cur_priority;
        } else if (s[operator_index[i]] == '.') {
            operator_priority[i] = 2 + cur_priority;
        } else if (s[operator_index[i]] == '*') {
            operator_priority[i] = 3 + cur_priority;
        }
    }

//    for (auto &elem: operator_priority) {
//        std::cout << elem << "  ";
//    }
//    std::cout << std::endl;

    return rec_tree(s, 0, s.size() - 1);
}


DFA re2dfa(const std::string &s) {
    std::string s1 = '(' + s + ")#";
//    std::cout << s1 << std::endl;

    for (int i = s1.size() - 1; i > 0; i--) {
        if (s1[i] != '*' && s1[i] != '|' && s1[i - 1] != '|' && s1[i] != ')' && s1[i - 1] != '(') {
            s1.insert(i, ".");
        }
    }
//    std::cout << s1 << std::endl;

    Tree *root = create_tree(s1);

    root->calculate_nullable_firstpos_lastpos();

    followpos = std::vector<std::set<int>>(char_index.size());
    std::vector<std::set<int>> &followpos1 = followpos;
    std::vector<int> &char_index1 = char_index;


    root->calculate_followpos();

//    for (auto &vect: followpos) {
//        for (auto &elem: vect) {
//            std::cout << elem << ' ';
//        }
//        std::cout << std::endl;
//    }



//    std::set<char> set1 = {'a','b','c'};
//    std::set<char> set2 = {'d','e','c'};
//
//    std::set<char> set3 = set1;
//    set3.insert(set2.begin(), set2.end());
//
//    for (auto &elem: set3) {
//        std::cout << elem << ' ';
//    }
//    std::cout << std::endl;

//    for (auto &elem: set2) {
//        std::cout << elem << ' ';
//    }
//    std::cout << std::endl;

    Alphabet alphabet = Alphabet(s);
    DFA res = DFA(alphabet);

    std::vector<std::set<int>> states;
    states.push_back(root->firstpos);

    res.create_state(std::to_string(0), root->nullable);
    res.set_initial(std::to_string(0));
    for (int index = 0; index < states.size(); index++) {
        for (auto &alpha_from_alphabet: alphabet) {
            std::set<int> new_state;
            for (auto &index_alpha_in_cur_state: states[index]) {
                if (s1[char_index[index_alpha_in_cur_state]] == alpha_from_alphabet) {
                    new_state.insert(followpos[index_alpha_in_cur_state].begin(),
                                     followpos[index_alpha_in_cur_state].end());
                }
            }

            if (!new_state.empty()) {
                int index_to = 0;
                for (; index_to < states.size(); index_to++) {
                    if (states[index_to] == new_state) {
                        break;
                    }
                } //existing state
                if (index_to == states.size()) {
                    //new state
                    index_to = states.size();
                    states.push_back(new_state);
                    res.create_state(std::to_string(index_to), new_state.count(char_index.size() - 1) == 1);
                }
                res.set_trans(std::to_string(index), alpha_from_alphabet, std::to_string(index_to));
            }
        }
    }


    return res;
}
