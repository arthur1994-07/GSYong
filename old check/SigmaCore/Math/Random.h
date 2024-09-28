#ifndef _SIGMA_CORE_RANDOM_H_
#define _SIGMA_CORE_RANDOM_H_

#pragma once
//#include <boost/tr1/random.hpp>
#include <ctime>
#include <random>
//#include <boost/random.hpp>

//#define RANDOM_POS	(((FLOAT)rand())/RAND_MAX) //! 0.f ~ 1.f
//#define sc::Random::RANDOM_NUM()	((RANDOM_POS*2.f)-1.f)   //! -1.f ~ 1.f


namespace sc
{
	// 100%  < float 정밀도에따라 0~99.9999% 까지 하기 위한 매직넘버이다.
	static const unsigned int	ACCURATE			= 1000000;	// 10의 배수로 해야됨
	static const float			ACCURATE_TO_FLOAT	= 10000;	// 0~99.9999%	ACCURATE * ACCURATE_TO_FLOAT
	static const float			ACCURATE_TO_RATE	= 1000000;	// 0~0.9999		ACCURATE * ACCURATE_TO_RATE
	

	namespace random
	{
		void GenerateSeed();
	}

	class Random
	{
	private:
		typedef std::tr1::random_device								Device;
		typedef std::tr1::mt19937									Engine;
		typedef std::tr1::uniform_int<unsigned int>					Distribution;
		typedef std::tr1::variate_generator<Engine, Distribution>	Generator;

		typedef std::tr1::shared_ptr<Generator>						GeneratorPtr;

		Random() 
		{
			Device device;
			Engine engine( device() );
			
			Distribution distribution( 0, ACCURATE-1 );					// 범위 설정
			spGenerator = GeneratorPtr(new Generator(engine, distribution)); // 생성기

		}
		virtual ~Random(){};
		GeneratorPtr	spGenerator;


	public:
		static Random& getInstance();

	protected:

 
        //! 1 ~ 9999
        //WORD get();
		//! 0 ~ 99
		//WORD getPercent();
		//! 0.01f ~ 99.99 f
		//float getFloatPercent();
		//! 0f ~ 0.9999f
		//float getFloatRate()
	public:
		// 경고! 정밀하지 않음.
		unsigned int GetValue()
		{
			return (*spGenerator)();
		}
		// 경고! 정밀하지 않음.
		unsigned int GetValue( DWORD nMin, DWORD nMax );
		/*
		{
			if ( nMax < nMin )
				return 0;
			else if ( nMax == nMin )
				return nMin;

			return ((*spGenerator)()%(nMax+1-nMin))+nMin;
		}
		*/
		// unsigned int 0 ~ 99
		unsigned int GetPercent()
		{
			return static_cast<int>( GetFloatPercent() );
		}

		unsigned int GetPercent_1000 ()
		{
			return (*spGenerator)()/1000;
		}

		unsigned int GetPercent_10000 ()
		{
			return (*spGenerator)()/100;
		}

		unsigned int GetPercent_100000 ()
		{
			return (*spGenerator)()/10;
		}

		// 0f ~ 99.9999f
        float GetFloatPercent()
		{
			return (*spGenerator)()/ACCURATE_TO_FLOAT;
		}

		//! 0f ~ 0.9999f
		float GetFloatRate()
		{
			return (*spGenerator)()/ACCURATE_TO_RATE;
		}

		/*
		 * in : 확률(0.0f ~ 100.0f);
		 * out : 성공/실패;
		 */
        BOOL IsSucceed(float fPercent)
		{
			return ( GetFloatPercent() < fPercent );
		}

        FORCEINLINE static float RandomNumber(float fMin, float fMax)
        {
			random::GenerateSeed();

            if(fMin == fMax)
                return fMin;
            float fRandom = (float) rand() / (float) RAND_MAX;
            return ((fRandom * (float)fabs(fMax-fMin))+fMin);
        }

        FORCEINLINE static int RandomNumber(int iMin, int iMax)
        {
			random::GenerateSeed();

            if (iMin == iMax)
                return iMin;
            else
                return ((rand() % (abs(iMax-iMin)+1))+iMin);
        }

        //! 0.f ~ 1.f
        FORCEINLINE static float RANDOM_POS()
        {
			random::GenerateSeed();

            return (((FLOAT) rand())/RAND_MAX);
        }

        //! -1.f ~ 1.f
        FORCEINLINE static float RANDOM_NUM()
        {
			random::GenerateSeed();

            return (((((FLOAT) rand())/RAND_MAX)*2.f)-1.f);
        }
    };

    class RandomInt
    {
    public:
        RandomInt();
        ~RandomInt();

    public:
        __int64 Int64();
    };
}

#endif // _SIGMA_CORE_RANDOM_H_