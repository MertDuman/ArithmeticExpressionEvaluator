#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h> 
#include <regex>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono; 

/**
 * An enum for describing the return values of the evaluation.
**/
enum arithmeticResult {
	NOT_ARITHMETIC,
	PAREN_NOT_BALANCED,
	DIVISION_BY_ZERO,
	ILLEGAL_EXPRESSION,
    TOO_MANY_OPERATORS,
    TOO_MANY_OPERANDS,
    NO_OPERANDS,
	SUCCESS
};

/**
 * Checks if the expression is arithmetic.
**/
bool isArithmeticExpression(string expr) {
    regex nonArith("[^-+*/%0-9\\. \\(\\)\n]");
    
    return !regex_search(expr, nonArith);
}
  
/**
 * Checks if the parenthesis in the expression are balanced.
 * i.e same number of left and right parenthesis appearing in correct order.
**/
bool areParenthesisBalanced(string expr) { 
    stack<char> s; 
    char x; 
  
    for (int i = 0; i < expr.length(); i++) { 
        if (expr[i] == '(') { 
            s.push(expr[i]); 
            continue; 
        } 

        if (expr[i] == ')') {
            if (s.size() == 0) {
                return false;
            }
            s.pop(); 
        }
    }
    
    return s.empty(); 
}

/**
 * Returns the length of the expression in between two parenthesis.
**/
int getLengthParenthesis(string expr, int startLoc) {
    int par = 1;
    for (int i = startLoc + 1; i < expr.length(); i++) {
        if (expr[i] == '(') {
            par++;
        } else if (expr[i] == ')') {
            par--;
        }
        
        if (par == 0) {
            return i - startLoc - 1;
        }
    }
    
    return -1;
}

/**
 * Calculates the value of the arithmetic expression. 
 * This expression is assumed to not have any parenthesis.
**/
arithmeticResult doArithmetic(string expr, float& result) {
    regex nums("([0-9]*)\\.([0-9]+)|([0-9]+)");
    
    smatch match;
    string temp;
    
    vector<float> floats;
    vector<string> operators;
    
    temp = expr;
    while(regex_search(temp, match, nums)) {
        floats.push_back(atof(match[0].str().c_str()));
        // cout << "Flo: " << match[0] << endl;
        temp = match.suffix().str();
    }
    
    if (floats.size() == 0) {
        return arithmeticResult::NO_OPERANDS;
    }
    
    int sawNumber = 0;
    string toPush;
    for (int i = 0; i < expr.length(); i++) {
        if (expr[i] == '-') {
            if (sawNumber == 0) {
                floats[operators.size()] = -1 * floats[operators.size()]; 
            } else {
                toPush = expr[i];
                operators.push_back(toPush);
                sawNumber = 0;
            }
        } else if (expr[i] == '+') {
            if (sawNumber == 1) {
                toPush = expr[i];
                operators.push_back(toPush);
                sawNumber = 0;
            }
        } else if (expr[i] == '*' || expr[i] == '/' || expr[i] == '%') {
            if (sawNumber == 0) {
                return arithmeticResult::ILLEGAL_EXPRESSION;
            } else {
                toPush = expr[i];
                operators.push_back(toPush);
                sawNumber = 0;
            }
        } else if (expr[i] == ' ') {
            
        } else {
            sawNumber = 1;
        }            
    }
    
    if (operators.size() >= floats.size()) {
        return arithmeticResult::TOO_MANY_OPERATORS;
    }
    
    if (floats.size() > operators.size() + 1) {
        return arithmeticResult::TOO_MANY_OPERANDS;
    }
    
    /* for (int i = 0; i < operators.size(); i++) {
        cout << "Ops: " << operators[i] << endl;
    } */
    
    int index = 0;
    int didOp = 1;
    int size = operators.size();
    for (int i = 0; i < size; i++) {
        float x = floats[index];
        float y = floats[index + 1];
        float res;
        
        if (operators[index] == "*") {
            res = x * y;
            operators.erase(operators.begin() + index);
        } else if (operators[index] == "/") {
            if (y == 0) {
                return arithmeticResult::DIVISION_BY_ZERO;
            }
            res = x / y;
            operators.erase(operators.begin() + index);
        } else if (operators[index] == "%") {
            res = (int)x % (int)y;
            operators.erase(operators.begin() + index);
        } else {
            didOp = 0;
            index++;
        }
        
        if (didOp == 1) {
            floats.erase(floats.begin() + index);
            floats.erase(floats.begin() + index);
            floats.insert(floats.begin() + index, res);
        }
        didOp = 1;
    }
    
    index = 0;
    didOp = 1;
    size = operators.size();
    for (int i = 0; i < size; i++) {
        float x = floats[index];
        float y = floats[index + 1];
        float res;
        
        if (operators[index] == "+") {
            res = x + y;
            operators.erase(operators.begin() + index);
        } else if (operators[index] == "-") {
            res = x - y;
            operators.erase(operators.begin() + index);
        } else {
            didOp = 0;
            index++;
        }
        
        if (didOp == 1) {
            floats.erase(floats.begin() + index);
            floats.erase(floats.begin() + index);
            floats.insert(floats.begin() + index, res);
        }
        didOp = 1;
    }
    
    result = floats[0];
    return arithmeticResult::SUCCESS;
}

/**
 * Recursively solves the expression between every parenthesis until no parenthesis are left, evemtually solving the expression itself.
**/
arithmeticResult evaluateArithmeticExpressionRecursive(string& expr, int startLoc, int length) {
    regex par("\\(");
    smatch match;
    int lengthDifference = 0;
    
    string sub = expr.substr(startLoc, length);
    // cout << "First sub: " << sub << endl;
    
    /* Calculate the length of the expression before going through any changes.
    Doing this here allows accumulation of differences (diff) between each recursive call. */
    int beforeLength = expr.length();
    
    /* Loop for each parenthesis in the string */
    while(regex_search(sub, match, par)) {
        /* Find the length of the expression between the parenthesis */
        int parenLength = getLengthParenthesis(expr, startLoc + match.position(0));
        
        /* Evaluate any parenthesis between this sub-section. */
		arithmeticResult eResult = evaluateArithmeticExpressionRecursive(expr, startLoc + match.position(0) + 1, parenLength);
        
		if (eResult != arithmeticResult::SUCCESS) {
			return eResult;
		}

        int afterLength = expr.length();
        
        lengthDifference = beforeLength - afterLength;
        
        // cout << "Diff: " << diff << endl;
        
        sub = expr.substr(startLoc, length - lengthDifference);
        // cout << "Sub: " << sub << endl;
    }
    
    /* If no parenthesis are left, do simple arithmetic */
	float result;
    arithmeticResult eResult = doArithmetic(expr.substr(startLoc, length - lengthDifference), result);
            
	if (eResult != arithmeticResult::SUCCESS) {
		return eResult;
	}

    /* Save the result as string */
    char buffer[32];
    sprintf(buffer, "%.2f", result);
    string temp(buffer);
    
    /* Update the original expression with the new float value
    i.e   (3 + 5) - 8   becomes   8 - 8 */
    if (startLoc != 0) {
        expr.replace(startLoc - 1, length + 2 - lengthDifference, temp);
    } else {
        expr = temp;
    }

    return arithmeticResult::SUCCESS;
}

/**
 * Wrapper call for arithmetic expression evaluator. Checks if the expression is suitable.
**/
arithmeticResult evaluateArithmeticExpression(string expr, float& result) {
    if (!isArithmeticExpression(expr)) {
        return arithmeticResult::NOT_ARITHMETIC;
    }
    
    if (!areParenthesisBalanced(expr)) {
        return arithmeticResult::PAREN_NOT_BALANCED;
    }
    
	arithmeticResult eResult = evaluateArithmeticExpressionRecursive(expr, 0, expr.length());

	if (eResult != arithmeticResult::SUCCESS) {
		return eResult;
	}

	result = atof(expr.c_str());
    return arithmeticResult::SUCCESS;
}

int main() {
    string expr = "0";
    while (expr != "q") {
        printf("Expression: ");
        getline(cin, expr);

		if (expr == "q") {
			break;
		}

        float result;
        auto start = high_resolution_clock::now(); 
		arithmeticResult eResult = evaluateArithmeticExpression(expr, result);
        auto end = high_resolution_clock::now();
        
        printf("Execution took %d microseconds.\n", duration_cast<microseconds>(end - start));
		switch (eResult) {
		case SUCCESS:
			printf("Result: %f\n", result);
			break;
		case DIVISION_BY_ZERO:
			printf("Division by zero!\n");
			break;
		case NOT_ARITHMETIC:
			printf("Expression is not arithmetic (or includes variable names)!\n");
			break;
		case PAREN_NOT_BALANCED:
			printf("Parenthesis are not balanced!\n");
			break;
		case ILLEGAL_EXPRESSION:
			printf("Illegal arithmetic expression!\n");
			break;
        case TOO_MANY_OPERATORS:
			printf("Too many operators!\n");
			break;
        case TOO_MANY_OPERANDS:
			printf("Too many operands!\n");
			break;
        case NO_OPERANDS:
			printf("No operands!\n");
			break;
		}
    }
    return 0;
}