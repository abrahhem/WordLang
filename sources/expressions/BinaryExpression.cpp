#include "../../headers/expressions/BinaryExpression.hpp"

BinaryExpression::BinaryExpression(Expression* l, Action a, Expression* r) : left(l), action(a), right(r){}

BinaryExpression::~BinaryExpression() {
	delete left;
	delete right;
}

Term BinaryExpression::evaluate() {
	switch (this->action) {
		case Action::plus: 
			return this->plusAction();
		case Action::minus: 
			return this->minusAction();
		case Action::concat: 
			return this->concatAction();
		case Action::index: 
			return this->indexAction();
		default:
			break;
	}
}


Term BinaryExpression::plusAction() {
	Term left_term = this->left->evaluate(), right_t = this->right->evaluate();

	if (left_term.getType() != Type::integer || right_t.getType() != Type::integer) 
		throw runtime_error("Invalid operands to + operator");
	
	int *result = new int(*(int*)left_term.getValue() + *(int*)right_t.getValue());
	return Term(result, Type::integer);
}

Term BinaryExpression::minusAction() {

	Term left_term = this->left->evaluate(), right_t = this->right->evaluate();

	Type left_type = left_term.getType();
	Type right_type = right_t.getType();
	
	void* left_v = left_term.getValue();
	void* right_v = right_t.getValue();


	switch (left_type)
	{
		case Type::integer: {
			switch (right_type)
			{
				case Type::integer: {
					int *result = new int(*(int*)left_v - *(int*)right_v);
					return Term(result, Type::integer);
				}
				case Type::character: {
					throw runtime_error("Invalid operands for '-': integer - character");
				}
				case Type::word: {
					throw runtime_error("Invalid operands for '-': integer - word");
				}
				case Type::sentence: {
					throw runtime_error("Invalid operands for '-': integer - sentence");
				}
				default:
					break;
			}
			break;
		}
		case Type::character: {
			switch (right_type)
			{
				case Type::integer: {
					throw runtime_error("Invalid operands for '-': character - integer");
				}
				case Type::character: {
					char *result = new char(*(char*)left_v);
					if (*(char*)left_v == *(char*)right_v)
						*result = '\0';
					return Term(result, Type::character);
				}
				case Type::word: {
					throw runtime_error("Invalid operands for '-': character - word");
				}
				case Type::sentence: {
					throw runtime_error("Invalid operands for '-': character - sentence");
				}
				default:
					break;
			}
			break;
		}
		case Type::word: {
			switch (right_type)
			{
				case Type::integer: {
					throw runtime_error("Invalid operands for '-': word - integer");
				}
				case Type::character: {
					string *result = new string(*(string*)left_v);
					size_t pos = result->find(*(char*)right_v);
					if (pos != string::npos) 
						result->erase(pos, 1);
					return Term(result, Type::word);
				}
				case Type::word: {
					string *result = new string(*(string*)left_v == *(string*)right_v ? "" : *(string*)left_v);
					return Term(result, Type::word);
				}
				case Type::sentence: {
					throw runtime_error("Invalid operands for '-': word - sentence");
				}
				default:
					break;
			}
			break;
		}
		case Type::sentence: {
			switch (right_type)
			{
				case Type::integer: {
					throw runtime_error("Invalid operands for '-': sentence - integer");
				}
				case Type::character: {
					string *result = new string(*(string*)left_v);
					size_t pos = result->find(*(char*)right_v);
					if (pos != string::npos) 
						result->erase(pos, 1);

					// Remove double white spaces
					size_t found = result->find("  ");  // Search for double spaces
					while (found != string::npos) {
						result->replace(found, 2, " ");   // Replace double spaces with a single space
						found = result->find("  ", found + 1); // Search for more double spaces
					}

					// Remove leading and trailing white spaces
					size_t start = result->find_first_not_of(" ");
					size_t end = result->find_last_not_of(" ");
					if (start != string::npos && end != string::npos) {
						*result = result->substr(start, end - start + 1);
					} else {
						*result = "\n";
					}

					return Term(result, Type::sentence);
				}
				case Type::word: {
					string word = *(string*)right_v;
					stringstream ss(*(string*)left_v);
					string token, *result = new string();
					bool removed = false;
					while (ss >> token) {
						if (token == word && !removed) {
							removed = true;
						} else {
							if (!result->empty()) {
								*result += " ";
							}
							*result += token;
						}
					}
					*result += '\n';
					return Term(result, Type::sentence);
				}
				case Type::sentence: {
					string *result = new string(*(string*)left_v == *(string*)right_v ? "" : *(string*)left_v);
					return Term(result, Type::sentence);
				}
				default:
					break;
			}
			break;
		}
		default:
			break;
	}

	throw runtime_error("Invalid operands to - operator");
}