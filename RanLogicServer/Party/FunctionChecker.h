#ifndef FunctionChecker_H
#define FunctionChecker_H

/**
    Boost::Function 을 비교 한다.
    일반적으로 Boost::Function 끼리는 == 비교를 할 수 없다.
    이는 Boost::Function 에 Bind 함수를 대입할 수 있기 때문에 단순 == 로는 비교할 수 없기 때문이다.
    그러므로 우리는 템플릿을 통해 비교를 수행 한다.
	
 */
template<class Fun, class FunType>
class FunctionChecker
{
public:
    explicit FunctionChecker(Fun &f) : callback(f) {}

    /**
        실제 비교를 수행 한다.
        단 f 기반으로 비교함에 주의 한다.
        만약 callback == f 로 변경하면 작동 되지 않는다. 이를 주의한다.

     */
    friend bool operator==(const FunType &lhs, const FunctionChecker &rhs) 
    {
        return lhs == rhs.callback;
    }

    /**
        인자가 반대일 경우도 동작하게 하기 위해 처리 한다.

     */
    friend bool operator==(const FunctionChecker &lhs, const FunType &rhs) 
    {
        return rhs == lhs;
    }

private:
    const Fun& callback;
};

/**
	컨테이너의 입력된 fun 와 동일한 Function 을 찾는다.
	
 */
template<class FunType, class Fun, class InIt>
InIt FindFunction(Fun fun, InIt firstIt, InIt endIt)
{
    return std::find(firstIt, endIt, FunctionChecker<Fun, FunType>(fun));
}

#endif