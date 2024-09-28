#ifndef FunctionChecker_H
#define FunctionChecker_H

/**
    Boost::Function �� �� �Ѵ�.
    �Ϲ������� Boost::Function ������ == �񱳸� �� �� ����.
    �̴� Boost::Function �� Bind �Լ��� ������ �� �ֱ� ������ �ܼ� == �δ� ���� �� ���� �����̴�.
    �׷��Ƿ� �츮�� ���ø��� ���� �񱳸� ���� �Ѵ�.
	
 */
template<class Fun, class FunType>
class FunctionChecker
{
public:
    explicit FunctionChecker(Fun &f) : callback(f) {}

    /**
        ���� �񱳸� ���� �Ѵ�.
        �� f ������� ���Կ� ���� �Ѵ�.
        ���� callback == f �� �����ϸ� �۵� ���� �ʴ´�. �̸� �����Ѵ�.

     */
    friend bool operator==(const FunType &lhs, const FunctionChecker &rhs) 
    {
        return lhs == rhs.callback;
    }

    /**
        ���ڰ� �ݴ��� ��쵵 �����ϰ� �ϱ� ���� ó�� �Ѵ�.

     */
    friend bool operator==(const FunctionChecker &lhs, const FunType &rhs) 
    {
        return rhs == lhs;
    }

private:
    const Fun& callback;
};

/**
	�����̳��� �Էµ� fun �� ������ Function �� ã�´�.
	
 */
template<class FunType, class Fun, class InIt>
InIt FindFunction(Fun fun, InIt firstIt, InIt endIt)
{
    return std::find(firstIt, endIt, FunctionChecker<Fun, FunType>(fun));
}

#endif