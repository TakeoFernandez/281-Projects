// Project identifier: 1CAEF3A0FEDD0DEC26BA9808C69D4D22A9962768

#include "evaluate.hpp"
#include <cstdint>
#include <vector>

std::int64_t evaluate(std::string const& expression) {
    std::vector<int64_t> nums;
    std::vector<char> operators;
    for(size_t i = 0; i < expression.size(); ++i){
        if(expression[i] == '+' || expression[i] == '-' || expression[i] == '*' || expression[i] == '/'){
            operators.push_back(expression[i]);
        }
        else{
            nums.push_back(static_cast<int64_t>(expression[i] - '0'));
        }

        if(nums.size() >= 2 && !operators.empty()){
            int64_t right = nums.back();
            nums.pop_back();
            int64_t left = nums.back();
            nums.pop_back();
            if(operators.back() == '+'){
                nums.push_back(left + right);
                operators.pop_back();
            }
            else if(operators.back() == '-'){
                nums.push_back(left - right);
                operators.pop_back();
            }
            else if(operators.back() == '*'){
                nums.push_back(left * right);
                operators.pop_back();
            }
            else{
                nums.push_back(static_cast<int>(left)/static_cast<int>(right));
                operators.pop_back();
            }
        }//if there are enough numbers to preform an operation
    }
return nums.back();
}
