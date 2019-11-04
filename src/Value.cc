#include <utility>

#include <iostream>

#include "Value.h"
#include "Environment.h"
#include "Context.h"
#include "Logger.h"

bool Value::equals(const Value *rhs) const {
    return (type == rhs->type);
}

bool NumberValue::equals(const Value *rhs) const {
    return Value::equals(rhs) && (value == ((NumberValue *) rhs)->value);
}

string Value::toString() const {
    ostringstream result;
    result << "Value(";
    result << ValueTypeStrings[(int) type];
    result << ")";
    return result.str();
}

bool operator==(const Value &lhs, const Value &rhs) {
    return lhs.equals(&rhs);
}

string NumberValue::toString() const {
    ostringstream result;
    result << "NumberValue(" << value << ")";
    return result.str();
}

string StringValue::toString() const {
    return string("string");
}

bool StringValue::equals(const Value *rhs) const {
    return Value::equals(rhs) && (value == ((const StringValue *) (rhs))->value);
}

string DictionaryValue::toString() const {
    ostringstream result;
    result << "DictionaryValue(" << value.size() << ")";
    return result.str();
}

DictionaryValue::DictionaryValue() : Value(ValueType::DICT), parent(nullptr) {}

string UserFunctionValue::toString() const {
    ostringstream result;
    result << "UserFunctionValue(" << "..." << ")";
    return result.str();
}

Value *UserFunctionValue::apply(const vector<Value *> &argsIn,
                                Environment *caller, unordered_map<wstring, Value *> *kwargsIn) const {
//    wcout << endl << L"apply" << endl;
    Value *bodyReturnValue = nullptr;
    Environment *env;
    env = parentEnv->newChildEnvironment();
    env->caller = caller;
    vector<Value *> args = argsIn;
//    wcout << L"eval params" << endl;
    do {
        if (bodyReturnValue != nullptr) {
            delete bodyReturnValue;
        }
//        wcout << L"start" << endl;

        if (params.size() > argsIn.size()) {
            ConsoleLogger().log("エラー：引数は足りません　")
                    ->log("必要は")->logLong((long) params.size())
                    ->log(" 渡したのは")->logLong((long) argsIn.size())->logEndl();
        }
//        wcout << L"valid"  << params.size() << endl;
        for (size_t i = 0; i < params.size(); i++) {
//            wcout << L"eval param " << i << endl;
//            wcout.flush();
//            wcout << L"eval param " << params[i] << endl;
//            wcout.flush();
            env->bind(params[i], args[i]);
        }
//        wcout << L"defaultParams" << paramsWithDefault.size() << endl;
        for (auto item : paramsWithDefault) {
            wstring left = item.first;
//            wcout << L"default item " << left << L":" << endl;
//            wcout << L"d" << endl;
//            wcout.flush();
            if (kwargsIn->count(item.first)) {
                env->bind(item.first, (*kwargsIn)[item.first]);
            } else {
                env->bind(item.first, item.second);
            }
        }
        if (hasVarKeywordArgs) {
            auto kwArgs = env->context->newDictionaryValue();
            if (kwargsIn) {
                for (const auto &arg : *kwargsIn) {
                    kwArgs->set(arg.first, arg.second);
                }
            }
            env->bind(varKeywordArgsParam, kwArgs);
        }
        if (hasVarArgs) {
            auto varArgs = env->context->newArrayValue();
            if (argsIn.size() > params.size()) {
                for (size_t i = params.size(); i < argsIn.size(); i++) {
                    varArgs->push(argsIn[i]);
                }
            }
            env->bind(varArgsParam, varArgs);
        }
        bodyReturnValue = env->eval(body, this);
        if (bodyReturnValue->type == ValueType::RETURN) {
            auto inner = ((ReturnValue *) bodyReturnValue)->inner;
            delete bodyReturnValue;
            return inner;
        }
        if (bodyReturnValue->type == ValueType::TAIL_CALL) {
            args = ((TailCallValue *) bodyReturnValue)->args;
            env->tailReset();
        }
    } while (bodyReturnValue->type == ValueType::TAIL_CALL);
    return bodyReturnValue;
}

void UserFunctionValue::setVarKeywordParam(wstring name) {
    hasVarKeywordArgs = true;
    varKeywordArgsParam = std::move(name);
}

void UserFunctionValue::setVarParam(wstring name) {
    hasVarArgs = true;
    varArgsParam = std::move(name);
}

string ArrayValue::toString() const {
    stringstream ss;
    ss << "Array(長さ=" << length() << ")";
    return ss.str();
}
