#pragma once

#include <array>
#include <vector>
#include <memory> // unique_ptr
#include <ctime> // time for random seed

#if defined(__clang__) // Couldn't find one for clang
#define FORCE_INLINE inline
#elif defined(__GNUC__) || defined(__GNUG__)
#define FORCE_INLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
#define FORCE_INLINE __forceinline
#endif

class OpenSimplexNoise {
	friend class StaticConstructor;
	
protected:
	// Contribution structs
	struct Contribution2
	{
	public:
		double dx, dy;
		int xsb, ysb;
		Contribution2 *Next;

		Contribution2(double multiplier, int _xsb, int _ysb);
		~Contribution2();
	};
	using pContribution2 = std::unique_ptr<Contribution2>;
	struct Contribution3 {
	public:
		double dx, dy, dz;
		int xsb, ysb, zsb;
		Contribution3 *Next;

		Contribution3(double multiplier, int _xsb, int _ysb, int _zsb);
		~Contribution3();
	};
	using pContribution3 = std::unique_ptr<Contribution3>;
	struct Contribution4 {
	public:
		double dx, dy, dz, dw;
		int xsb, ysb, zsb, wsb;
		Contribution4 *Next;
		
		Contribution4(double multiplier, int _xsb, int _ysb, int _zsb, int _wsb);
		~Contribution4();
	};
	using pContribution4 = std::unique_ptr<Contribution4>;

	// Constants
	static const double STRETCH_2D;
	static const double STRETCH_3D;
	static const double STRETCH_4D;
	static const double SQUISH_2D;
	static const double SQUISH_3D;
	static const double SQUISH_4D;
	static const double NORM_2D;
	static const double NORM_3D;
	static const double NORM_4D;

	std::array<unsigned char, 256> perm;
	std::array<unsigned char, 256> perm2D;
	std::array<unsigned char, 256> perm3D;
	std::array<unsigned char, 256> perm4D;

	static std::array<double, 16> gradients2D;
	static std::array<double, 72> gradients3D;
	static std::array<double, 256> gradients4D;

	static std::vector<Contribution2*> lookup2D;
	static std::vector<Contribution3*> lookup3D;
	static std::vector<Contribution4*> lookup4D;

	static std::vector<pContribution2> contributions2D;
	static std::vector<pContribution3> contributions3D;
	static std::vector<pContribution4> contributions4D;

	struct StaticConstructor {
		StaticConstructor();
	};
	static StaticConstructor staticConstructor;
	
	FORCE_INLINE static inline int FastFloor(double x);
	
public:
	OpenSimplexNoise();
	OpenSimplexNoise(int64_t seed);
	
	double Evaluate(double x, double y);
	double Evaluate(double x, double y, double z);
	double Evaluate(double x, double y, double z, double w);
};
