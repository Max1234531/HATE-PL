
#include <vector>
#include <string>
#include <stack>
#include <map>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <set>
#include "Lexer.cpp"
#include "TID.cpp"
using namespace std;

//"ID""LOGICAL OPERATOR""BOOLEAN VALUE""RESERVED""SIMPLE SIGN""INCREMENT""ASSIGNMENT OPERATOR"
//"COMPLEX SIGN"+++"*""COMPARE OPERATOR""OPERATION""CONST""STRING CONST"
//"{""}"")""]""[""BRACKETS""CINOUT"
//"FOR""WHILE""FOREACH""DO""TYPE""IF""ELIF""NEW""PROGRAM"">>""<<"


TID* GlobalTID = new TID();
TID* CurrentTID = GlobalTID;
GStack GlobalStack;
Global_Memory Memory;
Poliz poliz;

void program();
void block();
void block_code();
void defenition();
void cycle();
void nexpression();
void condition();
void expression();
void pr1();
void pr2();
void pr3();
void pr4();
void atom();
void interaction_list();
void function_call();
void function_creation();
void simple_expression();
void array_build();

string buff;
string buff_type;

static string prg = "";

int cur_ind = -1111;

string gc() {
	string s = NextLex(prg);
	buff_type = GetType(s);
	return s;
}

stack<int> arr_size;
stack<string> f_type;
string v_type = "";
stack <vector<string>> args;
stack<string> names;

void program() {
	if (buff_type == "PROGRAM") {
		buff = gc();
		if (buff_type == "ID") {
			buff = gc();
			block();
			if (buff_type != "END") {
				pair<string, string> p = make_pair(buff, "end of program");
				throw p;
			}
		}
		else {
			pair<string, string> p = make_pair(buff, "ID of program");
			throw p;
		}
	}
	else {
		pair <string, string> p = make_pair(buff, "program");
		throw p;
	}

}

void block() {
	if (CurrentTID->TID_name == "function_creation_block") {
		CurrentTID->TID_name = "function block";
	}
	else if (CurrentTID->TID_name != "for block" && CurrentTID->TID_name != "smth block") {
		CurrentTID = new TID(CurrentTID);
	}
	else {
		CurrentTID->TID_name = "used block";
	}

	if (buff_type == "{") {
		buff = gc();
		block_code();
		if (buff_type == "}") {
			if (CurrentTID->TID_name == "function block") {
				poliz.push("return_stack.top()", Type::service);
				poliz.push("goto", Type::service);
			}
			buff = gc();
			//CurrentTID->Delete_Table();
			//Memory.print();
			CurrentTID = CurrentTID->parent;
			return;

		}
		else {
			pair<string, string> p = make_pair(buff, "}");
			throw p;
		}
	}
	else {
		pair<string, string> p = make_pair(buff, "{");
		throw p;
	}
}



void defenition() {
	if (buff_type == "INT" || buff_type == "DOUBLE" || buff_type == "BOOL" || buff_type == "STRING") {
		if (CurrentTID->TID_name == "function_creation_block") {
			args.top().push_back(GlobalStack.get_type(buff_type));
		}
		if (args.top().size() == 0) f_type.push(buff_type);

		v_type = buff_type;

		buff = gc();
		if (buff_type == "ID") {
			names.push(buff);
			buff = gc();
			if (buff_type == "=") {

				if (!CurrentTID->check_here_v(names.top())) {
					CurrentTID->add_v(GlobalStack.get_type(v_type), names.top());
				}
				else {
					throw  "'" + names.top() + "' IS ALREADY DEFINED HERE";
				}
				poliz.analise("ID", to_string(CurrentTID->get_adress_v(names.top())), Type::var_link);///////////////////
				poliz.analise("=", "=", Type::operation);/////////////
				GlobalStack.push(GlobalStack.get_type(v_type));
				GlobalStack.push("=");
				names.pop();
				v_type = "";

				buff = gc();
				expression();
				if (!GlobalStack.check_bi()) { throw buff + " - TYPE ERROR"; };
				return;
			}
			else if (buff_type == "(") {
				int dash_here = poliz.get_current_index();
				poliz.dash();
				poliz.push("goto", Type::service);
				cur_ind = poliz.get_current_index();//////////////////////////////////
				//if (CurrentTID->TID_name == "function block") { throw buff + " - FUNCTION ARGUMENT DEFENITION EXPECTED"; }
				function_creation();
				//CurrentTID = new TID(CurrentTID, "function block");
				CurrentTID->return_type = (GlobalStack.get_type(f_type.top()));
				f_type.pop();
				block();
				poliz.replace_here(to_string(poliz.get_current_index()), Type::poliz_adress, dash_here);
				return;
			}
			else if (buff_type == "[") {
				//poliz.analise("[", "[", Type::operation);
				//expression();
				//poliz.add_to_answer();
				if (!CurrentTID->check_here_v(names.top())) {
					//////////////////////////////////////////////////////HEREHERE
					CurrentTID->add_v(GlobalStack.get_type(v_type), names.top());
					//names.pop();
					//Memory.blink(stoi(buff), GlobalStack.get_type(v_type));
					//v_type = "";//////////////////////add size
				}
				else {
					throw  "'" + names.top() + "' IS ALREADY DEFINED HERE";
				}

				poliz.analise("ID", names.top(), Type::arr_creation);
				poliz.analise("[", "[", Type::operation);
				array_build();
				poliz.fix_arr(names.top(), to_string(CurrentTID->get_adress_v(names.top())), arr_size.top());
				poliz.analise("]", "]", Type::operation);
				arr_size.pop();
				names.pop();

				return;
			}
			else {

				if (!CurrentTID->check_here_v(names.top())) {
					CurrentTID->add_v(GlobalStack.get_type(v_type), names.top());
					//poliz.analise("ID", names.top(), Type::var_link);///////////////////
					//poliz.analise("=", "=", Type::operation);
					//poliz.analise("CONST", "0", Type::const_);
					if (CurrentTID->TID_name == "function_creation_block") {
						poliz.analise("ID", to_string(CurrentTID->get_adress_v(names.top())), Type::var_link);
						poliz.analise("=", "=", Type::operation);
						poliz.analise("CONST", "args_stack.top()", Type::service);
						poliz.analise(";", ";", Type::operation);
					}

					names.pop();
					v_type = "";
				}
				else {
					throw  "'" + names.top() + "' IS ALREADY DEFINED HERE";
				}


				return;
			}
		}

		else if (buff_type == "*") {//////////////////////////////smth about poliz
			if (CurrentTID->TID_name == "function_creation_block") {
				args.top()[args.top().size() - 1] += "*";
				args.top()[args.top().size() - 1] = GlobalStack.get_type(args.top()[args.top().size() - 1]);

			}
			else {
				f_type.top() += "*";
			}

			v_type += "*";
			buff = gc();
			if (buff_type == "ID") {
				names.push(buff);
				buff = gc();
				if (buff_type == "=") {
					if (!CurrentTID->check_here_v(names.top())) {
						CurrentTID->add_v(GlobalStack.get_type(v_type), names.top());

					}
					else {
						throw  "'" + names.top() + "' IS ALREADY DEFINED HERE";
					}
					GlobalStack.push(CurrentTID->get_type_v(names.top()));
					GlobalStack.push("=");
					names.pop();
					v_type = "";
					buff = gc();
					if (buff_type == "NEW") {
						buff = gc();
						if (buff_type == "INT" || buff_type == "DOUBLE" || buff_type == "BOOL" || buff_type == "STRING") {
							GlobalStack.push(GlobalStack.get_type(buff_type + "*"));
							if (!GlobalStack.check_bi()) { throw buff + " - TYPE ERROR"; };
							buff = gc();
							return;
						}
						else {
							pair<string, string> p = make_pair(buff, "TYPE");
							throw p;
						}
					}
					else {
						expression();
						if (!GlobalStack.check_bi()) { throw buff + " - TYPE ERROR"; };
						return;
					}

				}
				else if (buff_type == "(") {
					function_creation();
					//CurrentTID = new TID(CurrentTID, "function block");
					CurrentTID->return_type = (GlobalStack.get_type(f_type.top()));
					f_type.pop();
					block();
					CurrentTID = CurrentTID->parent;
					return;
				}
				else { names.pop(); return; }
			}
			else {
				pair<string, string> p = make_pair(buff, "ID");
				throw p;
			}
		}
		else {
			pair<string, string> p = make_pair(buff, "ID");
			throw p;
		}
	}
	else if (buff_type == "VOID") {
		f_type.push(buff_type);
		buff = gc();
		if (buff_type == "ID") {
			names.push(buff);
			buff = gc();
			int dash_here = poliz.get_current_index();
			poliz.dash();
			poliz.push("goto", Type::service);
			cur_ind = poliz.get_current_index();//////////////////////////////////
			function_creation();
			//CurrentTID = new TID(CurrentTID, "function block");
			CurrentTID->return_type = (GlobalStack.get_type(f_type.top()));
			f_type.pop();
			block();
			poliz.replace_here(to_string(poliz.get_current_index()), Type::poliz_adress, dash_here);
			//CurrentTID = CurrentTID->parent;
			return;
		}
		else {
			pair<string, string> p = make_pair(buff, "ID of function");
			throw p;
		}
	}
	else if (buff_type == "ID") {
		names.push(buff);
		buff = gc();
		if (buff_type == "=" || buff_type == "-=" || buff_type == "+=" || buff_type == "/=" || buff_type == "*=") {
			if (!CurrentTID->check_v(names.top())) {
				throw "'" + names.top() + "' WAS NOT DECLARED IN THIS SCOPE";
			}
			GlobalStack.push(CurrentTID->get_type_v(names.top()));
			GlobalStack.push(buff_type);
			poliz.analise("ID", to_string(CurrentTID->get_adress_v(names.top())), Type::var_link);///////////////////
			poliz.analise(buff_type, buff, Type::operation);/////////////
			names.pop();
			buff = gc();
			expression();
			if (!GlobalStack.check_bi()) { throw buff + " - TYPE ERROR"; };
			return;
		}
		else if (buff_type == "(") {
			function_call();
		}
		else if (buff_type == "[") {
			/////////////////////////////////////////////////////////////////////////////////////////////
			//poliz.analise("[", "[", Type::operation);
			if (!CurrentTID->check_v(names.top())) {
				throw "'" + names.top() + "' WAS NOT DECLARED IN THIS SCOPE";
			}


			GlobalStack.push(CurrentTID->get_type_v(names.top()).substr(0, CurrentTID->get_type_v(names.top()).find('_')));

			poliz.analise("ID", names.top(), Type::indexation);
			poliz.analise("[", "[", Type::operation);
			array_build();
			poliz.fix_arr_call(names.top(), to_string(CurrentTID->get_adress_v(names.top())), arr_size.top());
			poliz.analise("]", "]", Type::operation);
			arr_size.pop();
			names.pop();

			if (buff_type == "=" || buff_type == "-=" || buff_type == "+=" || buff_type == "/=" || buff_type == "*=") {
				poliz.analise(buff_type, buff, Type::operation);
				GlobalStack.push(buff_type);
				buff = gc();
				expression();
				if (!GlobalStack.check_bi()) { throw buff + " - TYPE ERROR"; };
				return;
			}
			else {
				pair<string, string> p = make_pair(buff, "ASSIGNMENT OPERATOR");
				throw p;
			}
		}
		else {
			pair<string, string> p = make_pair(buff, "ANOTHER STRUCTURE OF DEFENITION");
			throw p;
		}
	}

	else if (buff_type == "*") {////////////////////////////////////////////////////////
		buff = gc();
		if (buff_type == "ID") {
			names.push(buff);
			buff = gc();
			if (!CurrentTID->check_v(names.top())) {
				throw "'" + names.top() + "' WAS NOT DECLARED IN THIS SCOPE";
			}
			GlobalStack.push(GlobalStack.get_type(names.top()));
			names.pop();
			if (buff_type == "=" || buff_type == "-=" || buff_type == "+=" || buff_type == "/=" || buff_type == "*=") {
				GlobalStack.push(buff_type);

				buff = gc();
				expression();
				if (!GlobalStack.check_bi()) { throw buff + " - TYPE ERROR"; };
				return;
			}
			else {
				pair<string, string> p = make_pair(buff, "ASSIGNMENT OPERATOR");
				throw p;
			}
		}
		else {
			pair<string, string> p = make_pair(buff, "ID");
			throw p;
		}

	}
	else {
		return;
		pair<string, string> p = make_pair(buff, "ANOTHER STRUCTURE OF DEFENITION");
		throw p;
	}
}


void array_build() {
	arr_size.push(0);
	while (buff_type == "[") {
		//poliz.analise("[", "[", Type::operation);
		buff = gc();
		expression();
		GlobalStack.push("=");
		GlobalStack.push("CONST");
		if (!GlobalStack.check_bi()) { throw buff + " - TYPE ERROR"; };
		poliz.analise(",,", ",,", Type::operation);
		arr_size.top() += 1;
		if (buff_type == "]") {
			//poliz.analise("[", "[", Type::operation);
			buff = gc();
		}
		else {
			pair<string, string> p = make_pair(buff, "]");
			throw p;
		}
	}
	return;
}

void function_creation() {/////////////////////////////////////////////////////////////////////////
	args.push({});

	CurrentTID = new TID(CurrentTID, "function_creation_block");
	TID* remember = CurrentTID;
	if (buff_type == "(") {
		buff = gc();
		if (buff_type == ")") {
			CurrentTID = CurrentTID->parent;
			if (!CurrentTID->check_here_f(names.top(), args.top())) {
				CurrentTID->add_f(GlobalStack.get_type(f_type.top()), args.top(), names.top(), cur_ind);
			}
			else {
				throw  "'" + names.top() + "' IS ALREADY DEFINED HERE";
			}
			names.pop();
			args.pop();
			buff = gc();
			CurrentTID = remember;
			return;
		}
		defenition();

		while (buff_type == ",") {
			buff = gc();
			defenition();
		}


		if (buff_type == ")") {
			CurrentTID = CurrentTID->parent;
			if (!CurrentTID->check_here_f(names.top(), args.top())) {
				CurrentTID->add_f(GlobalStack.get_type(f_type.top()), args.top(), names.top(), cur_ind);
			}
			else {
				throw  "'" + names.top() + "' IS ALREADY DEFINED HERE";
			}

			names.pop();
			args.pop();

			buff = gc();
			CurrentTID = remember;
			return;
		}
		else {
			pair<string, string> p = make_pair(buff, ")");
			throw p;
		}
	}
	else {
		pair<string, string> p = make_pair(buff, "(");
		throw p;
	}
}

void function_call() {
	args.push({});

	poliz.analise("ID", names.top(), Type::function_call);
	poliz.analise("(", "(", Type::operation);
	if (buff_type == "(") {
		buff = gc();
		if (buff_type == ")") {
			if (!CurrentTID->check_f(names.top(), args.top())) { throw "'" + names.top() + "' - WAS NOT DECLARED IN THIS SCOPE"; }
			GlobalStack.push(CurrentTID->get_type_f(names.top(), args.top()));

			poliz.fix_fun(names.top(), CurrentTID->get_name_f(names.top(), args.top()), args.top().size());
			poliz.analise(")", ")", Type::operation);
			names.pop();
			args.pop();
			buff = gc();

			return;
		}
		else {
			expression();
			args.top().push_back(GlobalStack.pop());

			while (buff_type == ",") {
				poliz.analise(",", ",", Type::operation);
				buff = gc();
				expression();
				args.top().push_back(GlobalStack.pop());
			}
			if (buff_type == ")") {
				if (!CurrentTID->check_f(names.top(), args.top())) { throw "'" + names.top() + "' - WAS NOT DECLARED IN THIS SCOPE"; }
				GlobalStack.push(CurrentTID->get_type_f(names.top(), args.top()));
				poliz.fix_fun(names.top(), CurrentTID->get_name_f(names.top(), args.top()), args.top().size());
				poliz.analise(")", ")", Type::operation);
				names.pop();
				args.pop();
				buff = gc();
				return;
			}
		}
	}
	else {
		pair<string, string> p = make_pair(buff, "(");
		throw p;
	}
}

void block_code() {
	if (buff_type == "}") {
		return;
	}
	else if (buff_type == "IF") {
		buff = gc();
		condition();
		block_code();
	}
	else if (buff_type == "CINOUT") {
		buff = gc();
		interaction_list();
		if (buff_type == ";") {
			buff = gc();
			block_code();
			return;
		}
		else {
			pair<string, string> p = make_pair(buff, ";");
			throw p;
		}
	}

	else if (buff_type == "FOR" || buff_type == "WHILE" || buff_type == "FOREACH" || buff_type == "DO") {
		cycle();
		block_code();

	}
	else if (buff_type == "RETURN") {
		//if (CurrentTID->TID_name != "function block") { throw buff + " - IN NONFUNCTIONAL BLOCK"; }
		buff = gc();
		GlobalStack.push(CurrentTID->return_type);
		GlobalStack.push("=");
		expression();
		if (!GlobalStack.check_bi()) { throw buff + " - TYPE ERROR"; }



		if (buff_type == ";") {
			poliz.analise(";", ";", Type::operation);
			poliz.push("return_stack.top()", Type::service);
			poliz.push("goto", Type::service);
			buff = gc();
			block_code();
		}
		else {
			pair<string, string> p = make_pair(buff, ";");
			throw p;
		}
	}
	else if (buff_type == "BREAK") {
		buff = gc();
		poliz.push("break", Type::service);
		poliz.push("goto", Type::service);
		if (buff_type == ";") {
			buff = gc();
			block_code();
		}

	}
	else if (buff_type == "CONTINUE") {
		buff = gc();
		poliz.push("continue", Type::service);
		poliz.push("goto", Type::service);
		if (buff_type == ";") {
			buff = gc();
			block_code();
		}
	}

	else {
		defenition();
		if (buff_type == ";") {
			poliz.analise(buff_type, buff, Type::operation);/////////////
			buff = gc();
			block_code();
		}
		else {
			pair<string, string> p = make_pair(buff, ";");
			throw p;
		}
	}
}

void cycle() {
	if (buff_type == "FOR") {
		CurrentTID = new TID(CurrentTID, "for block");
		buff = gc();
		if (buff_type == "(") {
			buff = gc();
			defenition();
			poliz.add_to_answer();
			if (buff_type == ";") {
				buff = gc();
				int cond = poliz.get_current_index();
				expression();
				poliz.add_to_answer();
				int dash_end = poliz.get_current_index();
				poliz.dash();//dead end
				poliz.push("goto_F", Type::service);
				int dash_bl = poliz.get_current_index();
				poliz.dash();//block
				poliz.push("goto", Type::service);
				/// <summary>
				/// ///////////
				/// </summary>
				string test = GlobalStack.pop();
				if (test != "CONST" && test != "VOID") { throw "'" + buff + "' - TYPE ERROR"; }
				//if (GlobalStack.pop() != "CONST") { throw "'" + buff + "' - TYPE ERROR"; }
				if (buff_type == ";") {
					buff = gc();
					int check = poliz.get_current_index();
					defenition();
					poliz.add_to_answer();
					poliz.push(to_string(cond), Type::poliz_adress);
					poliz.push("goto", Type::service);
					int bl = poliz.get_current_index();
					poliz.replace_here(to_string(bl), Type::poliz_adress, dash_bl);
					if (buff_type == ")") {
						//CurrentTID = CurrentTID->parent;
						buff = gc();
						block();
						poliz.push(to_string(check), Type::poliz_adress);
						poliz.push("goto", Type::service);
						int end = poliz.get_current_index();
						poliz.replace_here(to_string(end), Type::poliz_adress, dash_end);
						for (int i = cond; i < end; i++) {
							if (poliz.value_here(i) == "continue") { poliz.replace_here(to_string(check), Type::poliz_adress, i); }
							if (poliz.value_here(i) == "break") { poliz.replace_here(to_string(end), Type::poliz_adress, i); }
						}
						return;
					}
					else {
						pair<string, string> p = make_pair(buff, ")");
						throw p;
					}
				}
				else {
					pair<string, string> p = make_pair(buff, ";");
					throw p;
				}
			}
			else {
				pair<string, string> p = make_pair(buff, ";");
				throw p;
			}
		}
		else {
			pair<string, string> p = make_pair(buff, "(");
			throw p;
		}
	}
	else if (buff_type == "WHILE") {
		buff = gc();
		if (buff_type == "(") {
			buff = gc();
			int check = poliz.get_current_index();
			expression();
			if (GlobalStack.pop() != "CONST") { throw "'" + buff + "' - TYPE ERROR"; }
			//if (GlobalStack.pop() != "CONST") { throw "'" + buff + "' - TYPE ERROR"; }
			poliz.add_to_answer();
			int dash_end = poliz.get_current_index();
			poliz.dash();//dead end
			poliz.push("goto_F", Type::service);
			if (buff_type == ")") {
				buff = gc();
				block();
				poliz.push(to_string(check), Type::poliz_adress);
				poliz.push("goto", Type::service);
				poliz.replace_here(to_string(poliz.get_current_index()), Type::poliz_adress, dash_end);
				for (int i = check; i < poliz.get_current_index(); i++) {
					if (poliz.value_here(i) == "continue") { poliz.replace_here(to_string(check), Type::poliz_adress, i); }
					if (poliz.value_here(i) == "break") { poliz.replace_here(to_string(poliz.get_current_index()), Type::poliz_adress, i); }
				}
				return;
			}
			else {
				pair<string, string> p = make_pair(buff, ")");
				throw p;
			}
		}
		else {
			pair<string, string> p = make_pair(buff, "(");
			throw p;
		}
	}
	//else if (buff_type ==  "FOREACH") {///////////////////////////////////////////////////////////////
	//	CurrentTID = new TID(CurrentTID);
	//	buff = gc();
	//	if (buff_type ==  "(") {
	//		buff = gc();
	//		if (buff_type ==  "INT" || buff_type ==  "DOUBLE" || buff_type ==  "BOOL"  || buff_type == "STRING") {
	//			v_type = buff_type;
	//			buff = gc();
	//			if (buff_type ==  "ID") {
	//				names.push(buff);
	//				if (!CurrentTID->check_v(names.top())) {
	//					CurrentTID->add_v(GlobalStack.get_type(v_type), names.top());
	//				}
	//				else {
	//					throw  "'" + names.top() + "' IS ALREADY DEFINED HERE";
	//				}
	//				GlobalStack.push(CurrentTID->get_type_v(names.top()));
	//				v_type = "";
	//				names.pop();
	//				buff = gc();
	//				if (buff_type ==  "IN") {
	//					GlobalStack.push("=");
	//					buff = gc();
	//					if (buff_type ==  "ID") {
	//						names.push(buff);
	//						if (!CurrentTID->check_v(names.top())) {
	//							throw "'" + names.top() + "' WAS NOT DECLARED IN THIS SCOPE";
	//						}
	//						GlobalStack.push(CurrentTID->get_type_v(names.top()));
	//						names.pop();
	//						if (!GlobalStack.check_bi()) { throw "'" + buff + "' TYPE ERROR"; };
	//						buff = gc();
	//						if (buff_type ==  ")") {
	//							CurrentTID = CurrentTID->parent;
	//							buff = gc();
	//							block();
	//							return;
	//						}
	//						else {
	//							pair<string, string> p = make_pair(buff, ")");
	//							throw p;
	//						}
	//					}
	//					else {
	//						pair<string, string> p = make_pair(buff, "ID");
	//						throw p;
	//					}
	//				}
	//				else {
	//					pair<string, string> p = make_pair(buff, "in");
	//					throw p;
	//				}
	//			}
	//			else {
	//				pair<string, string> p = make_pair(buff, "ID");
	//				throw p;
	//			}
	//		}
	//		else {
	//			pair<string, string> p = make_pair(buff, "(");
	//			throw p;
	//		}
	//	}
	//	else {
	//		pair<string, string> p = make_pair(buff, "test");
	//		throw p;
	//	}
	//}
	else if (buff_type == "DO") {
		buff = gc();
		int rem = poliz.get_current_index();
		block();
		if (buff_type == "WHILE") {
			buff = gc();
			if (buff_type == "(") {
				buff = gc();
				expression();
				if (GlobalStack.pop() != "CONST") { throw "'" + buff + "' - TYPE ERROR"; }
				if (buff_type == ")") {
					poliz.add_to_answer();
					int dash_end = poliz.get_current_index();
					poliz.dash();//dead end
					poliz.push("goto_F", Type::service);
					poliz.push(to_string(rem), Type::poliz_adress);
					poliz.push("goto", Type::service);
					poliz.replace_here(to_string(poliz.get_current_index()), Type::poliz_adress, dash_end);
					buff = gc();
					if (buff_type == ";") {
						buff = gc();
						/*for (int i = check; i < dash_end; i++) {
							if (poliz.value_here(i) == "continue") { poliz.replace_here(to_string(check), Type::poliz_adress, i); }
							if (poliz.value_here(i) == "break") { poliz.replace_here(to_string(dash_end), Type::poliz_adress, i); }
						}
						return;*/
					}
					else {
						pair<string, string> p = make_pair(buff, ";");
						throw p;
					}
					return;
				}
				else {
					pair<string, string> p = make_pair(buff, ")");
					throw p;
				}
			}
			else {
				pair<string, string> p = make_pair(buff, "(");
				throw p;
			}
		}
		else {
			pair<string, string> p = make_pair(buff, "while");
			throw p;
		}
	}
	else {
		pair<string, string> p = make_pair(buff, "do");
		throw p;
	}
}

void condition() {
	vector<int> dead_end;
	if (buff_type == "(") {
		buff = gc();
		expression();
		poliz.add_to_answer();
		if (GlobalStack.pop() != "CONST") { throw "'" + buff + "' - TYPE ERROR"; }
		if (buff_type == ")") {
			int dash_here = poliz.get_current_index();
			poliz.dash();
			poliz.push("goto_F", Type::service);
			buff = gc();
			block();
			//S'
			dead_end.push_back(poliz.get_current_index());
			poliz.dash();
			poliz.push("goto", Type::service);
			while (buff_type == "ELIF") {
				poliz.replace_here(to_string(poliz.get_current_index()), Type::poliz_adress, dash_here);
				buff = gc();
				if (buff_type == "(") {
					buff = gc();
					expression();
					poliz.add_to_answer();
					if (GlobalStack.pop() != "CONST") { throw "'" + buff + "' - TYPE ERROR"; }
					dash_here = poliz.get_current_index();
					poliz.dash();
					poliz.push("goto_F", Type::service);
					if (buff_type == ")") {
						buff = gc();
						block();
						dead_end.push_back(poliz.get_current_index());
						poliz.dash();
						poliz.push("goto", Type::service);
					}
					else {
						pair<string, string> p = make_pair(buff, ")");
						throw p;
					}
				}
				else {
					pair<string, string> p = make_pair(buff, "(");
					throw p;
				}

			}
			if (buff_type == "ELSE") {
				poliz.replace_here(to_string(poliz.get_current_index()), Type::poliz_adress, dash_here);
				buff = gc();
				block();
				for (int i : dead_end) {
					poliz.replace_here(to_string(poliz.get_current_index()), Type::poliz_adress, i);
				}
				return;
			}
			else {
				for (int i : dead_end) {
					poliz.replace_here(to_string(poliz.get_current_index()), Type::poliz_adress, i);
				}
				poliz.replace_here(to_string(poliz.get_current_index()), Type::poliz_adress, dash_here);
				return;
			}

		}
		else {
			pair<string, string> p = make_pair(buff, ")");
			throw p;
		}
	}
	else {
		pair<string, string> p = make_pair(buff, "(");
		throw p;
	}
}


void simple_expression() {
	if (buff_type == "ENDL") {
		buff = "'\n'";
		buff_type = "STRING CONST";
	}
	if (buff_type == "STR" || buff_type == "DBL" || buff_type == "INT") {
		string ttype = buff_type;
		if (buff_type == "STR") {
			ttype = "STRING";
		}
		else if (buff_type == "DBL") {
			ttype = "DCONST";
		}
		else {
			ttype = "CONST";
		}


		poliz.analise("ID", buff_type, Type::type_change, 1);
		buff = gc();
		if (buff_type == "(") {
			poliz.analise("(", "(", Type::operation);
			buff = gc();
			expression();
			GlobalStack.pop();
			GlobalStack.push(ttype);
			if (buff_type == ")") {
				poliz.analise(buff_type, buff, Type::operation);/////////////
				buff = gc();
				return;
			}
			else {
				pair<string, string> p = make_pair(buff, ")");
				throw p;
			}
		}
		else {
			pair<string, string> p = make_pair(buff, "(");
			throw p;
		}
	}

	else if (buff_type == "CONST" || buff_type == "DCONST" || buff_type == "STRING CONST" || buff_type == "TRUE" || buff_type == "FALSE") {
		if (buff_type == "CONST" || buff_type == "TRUE" || buff_type == "FALSE") {
			GlobalStack.push("CONST");
		}
		else if (buff_type == "STRING CONST") {
			GlobalStack.push("STRING");
		}
		else {
			GlobalStack.push(buff_type);
		}
		if (buff_type == "CONST") poliz.analise(buff_type, buff, Type::int_const);
		else if (buff_type == "DCONST") poliz.analise(buff_type, buff, Type::double_const);
		else if (buff_type == "STRING CONST") poliz.analise(buff_type, buff, Type::string_const);
		else if (buff_type == "TRUE") poliz.analise(buff_type, "1", Type::int_const);
		else if (buff_type == "FALSE") poliz.analise(buff_type, "0", Type::int_const);

		buff = gc();
		return;
	}
	else if (buff_type == "ID") {

		string arr_name = buff;
		names.push(buff);
		buff = gc();
		if (buff_type == "(") {
			function_call();
			return;
		}
		else if (buff_type == "[") {
			if (!CurrentTID->check_v(names.top())) {
				throw "'" + names.top() + "' WAS NOT DECLARED IN THIS SCOPE";
			}
			GlobalStack.push(CurrentTID->get_type_v(names.top()).substr(0, CurrentTID->get_type_v(names.top()).find('_')));

			poliz.analise("ID", names.top(), Type::indexation_value);
			poliz.analise("[", "[", Type::operation);
			array_build();
			poliz.fix_arr_call_value(names.top(), to_string(CurrentTID->get_adress_v(names.top())), arr_size.top());
			poliz.analise("]", "]", Type::operation);
			arr_size.pop();
			names.pop();

			return;
		}
		if (!CurrentTID->check_v(names.top())) {
			throw "'" + names.top() + "' WAS NOT DECLARED IN THIS SCOPE";
		}

		GlobalStack.push(CurrentTID->get_type_v(names.top()));
		poliz.analise("ID", to_string(CurrentTID->get_adress_v(names.top())), Type::var_value);
		names.pop();
		return;
	}
	else if (buff_type == "*") {
		buff = gc();
		if (buff_type == "ID") {
			names.push(buff);
			if (!CurrentTID->check_v(names.top())) {
				throw "'" + names.top() + "' WAS NOT DECLARED IN THIS SCOPE";
			}
			GlobalStack.push(CurrentTID->get_type_v(names.top()));
			names.pop();
			buff = gc();
			return;
		}
		else {
			pair<string, string> p = make_pair(buff, "ID");
			throw p;
		}
	}
	else if (buff_type == "INCREMENT") {////////////////////////////////////////
		GlobalStack.push(buff_type);
		buff = gc();
		if (buff_type == "ID") {
			names.push(buff);
			if (!CurrentTID->check_v(names.top())) {
				throw "'" + names.top() + "' WAS NOT DECLARED IN THIS SCOPE";
			}
			poliz.analise("ID", to_string(CurrentTID->get_adress_v(names.top())), Type::var_value);///////////////////
			GlobalStack.push(CurrentTID->get_type_v(names.top()));
			names.pop();
			if (!GlobalStack.check_un()) { throw buff + " - TYPE ERROR"; };
			buff = gc();
			return;
		}
		else {
			pair<string, string> p = make_pair(buff, "ID");
			throw p;
		}
	}
	else {
		GlobalStack.push("VOID");
		poliz.analise("CONST", "1", Type::int_const);
		return;
		pair<string, string> p = make_pair(buff, "ANOTHER EXPRESSION");
		throw p;
	}
};


void expression() {
	nexpression();
	if (buff_type == "COMPARE OPERATOR") {
		GlobalStack.push(buff_type);
		poliz.analise(buff_type, buff, Type::operation);/////////////
		buff = gc();
		nexpression();
		if (!GlobalStack.check_bi()) { throw buff + " - TYPE ERROR"; };
		return;
	}
	return;
}

void nexpression() {
	pr1();
	while (buff_type == "OR") {
		poliz.analise(buff_type, buff, Type::operation);/////////////
		GlobalStack.push(buff_type);
		buff = gc();
		pr1();
		if (!GlobalStack.check_bi()) { throw buff + " - TYPE ERROR"; };
	}
	return;
}

void pr1() {
	pr2();
	while (buff_type == "AND") {
		poliz.analise(buff_type, buff, Type::operation);/////////////
		GlobalStack.push(buff_type);
		buff = gc();
		pr2();
		if (!GlobalStack.check_bi()) { throw buff + " - TYPE ERROR"; };
	}
	return;
}

void pr2() {
	pr3();
	while (buff_type == "SIMPLE SIGN") {
		poliz.analise(buff_type, buff, Type::operation);/////////////
		GlobalStack.push(buff_type);
		buff = gc();
		pr3();
		if (!GlobalStack.check_bi()) { throw buff + " - TYPE ERROR"; };
	}
	return;
}

void pr3() {
	pr4();
	while (buff_type == "COMPLEX SIGN" || buff_type == "*") {
		poliz.analise(buff_type, buff, Type::operation);/////////////
		GlobalStack.push("COMPLEX SIGN");
		buff = gc();
		pr4();
		if (!GlobalStack.check_bi()) { throw buff + " - TYPE ERROR"; };
	}
	return;
}

void pr4() {
	atom();
	if (buff_type == "^") {
		poliz.analise(buff_type, buff, Type::operation);/////////////
		GlobalStack.push(buff_type);
		buff = gc();
		atom();
		if (!GlobalStack.check_bi()) { throw buff + " - TYPE ERROR"; };
		return;
	}
	else if (buff_type == "INCREMENT") {
		poliz.analise(buff_type, buff, Type::operation);/////////////
		GlobalStack.push(buff_type);
		if (!GlobalStack.check_un()) { throw buff + " - TYPE ERROR"; };
		buff = gc();
		return;
	}
}

void atom() {
	if (buff_type == "(") {
		poliz.analise(buff_type, buff, Type::operation);/////////////
		buff = gc();
		expression();
		if (buff_type == ")") {
			poliz.analise(buff_type, buff, Type::operation);/////////////
			buff = gc();
			return;
		}
		else {
			pair<string, string> p = make_pair(buff, ")");
			throw p;
		}
	}

	else if (buff_type == "!" || buff_type == "SIMPLE SIGN") {
		if (buff_type == "SIMPLE SIGN") {
			poliz.analise("CONST", "0", Type::int_const);
		}

		poliz.analise(buff_type, buff, Type::operation);/////////////
		GlobalStack.push(buff_type);
		buff = gc();
		atom();
		if (!GlobalStack.check_un()) { throw buff + " - TYPE ERROR"; };
		return;
	}
	else {
		simple_expression();
		return;
	}
}


void interaction_list() {
	if (buff != ">>" && buff != "<<") {
		pair<string, string> p = make_pair(buff, ">> or <<");
		throw p;
	}
	while (buff_type == ">>" || buff_type == "<<") {
		if (buff_type == ">>") {

			buff = gc();
			if (buff_type == "ID") {
				if (!CurrentTID->check_v(buff)) {
					throw "'" + buff + "' WAS NOT DECLARED IN THIS SCOPE";
				}
				poliz.analise("ID", to_string(CurrentTID->get_adress_v(buff)), Type::var_link);
				poliz.add_to_answer();
				poliz.push(">>", Type::service);
				buff = gc();
				while (buff_type == ">>") {
					buff = gc();
					if (buff_type == "ID") {
						if (!CurrentTID->check_v(buff)) {
							throw "'" + buff + "' WAS NOT DECLARED IN THIS SCOPE";
						}
						poliz.analise("ID", to_string(CurrentTID->get_adress_v(buff)), Type::var_link);
						poliz.add_to_answer();
						poliz.push(">>", Type::service);
						buff = gc();

					}
				}
			}
			else { pair<string, string> p = make_pair(buff, "test"); }
		}
		else if (buff_type == "<<") {
			buff = gc();
			expression();
			poliz.add_to_answer();
			poliz.push("<<", Type::service);
			GlobalStack.pop();
			while (buff_type == "<<") {
				buff = gc();
				expression();
				poliz.add_to_answer();
				poliz.push("<<", Type::service);
				GlobalStack.pop();
			}
		}
	}
	return;

}



int main() {
	setlocale(LC_CTYPE, "rus");
	args.push({});

	ifstream file;
	file.open("FILES/Program.txt");

	while (!file.eof()) {
		prg += file.get();
		//getline(file, prg);
	}
	file.close();

	try {
		buff = gc();
		program();
		cout << "---------------------" << endl;
		cout << "ANALISE COMPLETED WITH NO ERRORS" << endl;
		//Memory.print();
		cout << "---------------------" << endl;
		//cout << poliz.im_callin_de_polis() << endl;
		cout << poliz.im_callin_de_simple_polis();
		cout << "---------------------" << endl;
		cout << "PROGRAM OUTPUT:" << endl;
		poliz.release_poliz();
		cout << endl << "---------------------" << endl;
		//Memory.print();
		return 0;
	}
	catch (pair<string, string> err) {
		cout << "---------------------" << endl;
		cout << "(ERR -> line:" << line_counter / 2 + 1 << ") with lex '" << err.first << "': " << '"' << err.second << '"' << " expected here" << endl;
		return 1;
	}
	catch (string err) {
		cout << "---------------------" << endl;
		cout << "(ERR -> line:" << line_counter / 2 + 1 << "): " << err << endl;
		return 1;
	}

	//1111
}