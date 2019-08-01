#if zlibwapitest_optimization_debug == 1
#elif zlibwapitest_optimization_release == 1
#else
#error zlibwapitest_optimization
#endif

#if zlibwapitest_platform_32bit == 1
#elif zlibwapitest_platform_64bit == 1
#else
#error zlibwapitest_platform
#endif

#if zlibwapitest_configuration_lib_mt == 1
#elif zlibwapitest_configuration_lib_md == 1
#elif zlibwapitest_configuration_dll_static == 1
#elif zlibwapitest_configuration_dll_runtime == 1
#else
#error zlibwapitest_configuration
#endif


#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <cstddef>
#include <iterator>

#include <memory>
#include <cassert>

#include <zlib.h>

#include <windows.h>


struct library_deleter
{
	void operator()(void* const& module_base_ptr) const;
};


typedef std::unique_ptr<void, library_deleter> unique_library;


static constexpr char const s_data_1[] = "Hello";
static constexpr char const s_data_2[] = " ";
static constexpr char const s_data_3[] = "world!";
static constexpr unsigned long s_checksum = 0x1d09045e;
static constexpr char const s_long_data[] = "aaaaaaaaaa--aaaaaaaaaa--aaaaaaaaaa--aaaaaaaaaa--aaaaaaaaaa";


HMODULE load_library(wchar_t const* const& library_name);
unique_library make_unique_library(HMODULE const& library_handle);
unique_library load_zlib_dll();
bool tests(unique_library const& zlib);
bool test_adler32(unique_library const& zlib);
bool test_adler32_combine(unique_library const& zlib);
bool test_adler32z(unique_library const& zlib);
bool test_compress(unique_library const& zlib);
bool test_compress2(unique_library const& zlib);
bool test_compressBound(unique_library const& zlib);


#define STRINGIFY(X) STRINGIFY_(X)
#define STRINGIFY_(X) #X

#if zlibwapitest_platform_32bit == 1

#if zlibwapitest_configuration_lib_mt == 1
#define get_proc_address(LIB, NAME, SUFFIX) (&(NAME))
#elif zlibwapitest_configuration_lib_md == 1
#define get_proc_address(LIB, NAME, SUFFIX) (&(NAME))
#elif zlibwapitest_configuration_dll_static == 1
#define get_proc_address(LIB, NAME, SUFFIX) (&(NAME))
#elif zlibwapitest_configuration_dll_runtime == 1
#define get_proc_address(LIB, NAME, SUFFIX) (GetProcAddress(reinterpret_cast<HMODULE>((LIB).get()), "_" #NAME "@" #SUFFIX))
#else
#error zlibwapitest_configuration
#endif

#elif zlibwapitest_platform_64bit == 1

#if zlibwapitest_configuration_lib_mt == 1
#define get_proc_address(LIB, NAME, SUFFIX) (&(NAME))
#elif zlibwapitest_configuration_lib_md == 1
#define get_proc_address(LIB, NAME, SUFFIX) (&(NAME))
#elif zlibwapitest_configuration_dll_static == 1
#define get_proc_address(LIB, NAME, SUFFIX) (&(NAME))
#elif zlibwapitest_configuration_dll_runtime == 1
#define get_proc_address(LIB, NAME, SUFFIX) (GetProcAddress(reinterpret_cast<HMODULE>((LIB).get()), #NAME))
#else
#error zlibwapitest_configuration
#endif

#else
#error zlibwapitest_platform
#endif


int main()
{
	unique_library const zlib = load_zlib_dll();
	if(!zlib)
	{
		std::printf("Could not load zlib.\n");
		return EXIT_FAILURE;
	}
	if(!tests(zlib))
	{
		std::printf("Tests failed.\n");
		return EXIT_FAILURE;
	}
	std::printf("Success.\n");
	return EXIT_SUCCESS;
}


void library_deleter::operator()(void* const& module_base_ptr) const
{
#if zlibwapitest_configuration_lib_mt == 1
	return;
#elif zlibwapitest_configuration_lib_md == 1
	return;
#elif zlibwapitest_configuration_dll_static == 1
	return;
#elif zlibwapitest_configuration_dll_runtime == 1
	BOOL const freed = FreeLibrary(reinterpret_cast<HMODULE>(module_base_ptr));
	assert(freed != 0);
#else
#error zlibwapitest_configuration
#endif
}

HMODULE load_library(wchar_t const* const& library_name)
{
#if zlibwapitest_configuration_lib_mt == 1
	return reinterpret_cast<HMODULE>(0x1);
#elif zlibwapitest_configuration_lib_md == 1
	return reinterpret_cast<HMODULE>(0x1);
#elif zlibwapitest_configuration_dll_static == 1
	return GetModuleHandleW(library_name);
#elif zlibwapitest_configuration_dll_runtime == 1
	return LoadLibraryW(library_name);
#else
#error zlibwapitest_configuration
#endif
}

unique_library make_unique_library(HMODULE const& library_handle)
{
	return unique_library(reinterpret_cast<void*>(library_handle));
}

unique_library load_zlib_dll()
{
	return make_unique_library(load_library(L"zlibwapi.dll"));
}

bool tests(unique_library const& zlib)
{
	typedef bool(*test_fn_t)(unique_library const&);
	static constexpr test_fn_t const s_test_fns[] =
	{
		&test_adler32,
		&test_adler32_combine,
		&test_adler32z,
		&test_compress,
		&test_compress2,
		&test_compressBound,
	};
	for(auto const& test_fn : s_test_fns)
	{
		if(!test_fn(zlib))
		{
			return false;
		}
	}
	return true;
}

bool test_adler32(unique_library const& zlib)
{
	auto const adler32_proc = get_proc_address(zlib, adler32, 12);
	auto const adler32_fn = reinterpret_cast<std::uint32_t(__stdcall*)(std::uint32_t, unsigned char const*, std::uint32_t)>(adler32_proc);
	if(!adler32_fn)
	{
		return false;
	}
	auto adler_crc = adler32_fn(0, nullptr, 0);
	adler_crc = adler32_fn(adler_crc, reinterpret_cast<unsigned char const*>(static_cast<char const*>(s_data_1)), static_cast<std::uint32_t>(std::size(s_data_1) - 1));
	adler_crc = adler32_fn(adler_crc, reinterpret_cast<unsigned char const*>(static_cast<char const*>(s_data_2)), static_cast<std::uint32_t>(std::size(s_data_2) - 1));
	adler_crc = adler32_fn(adler_crc, reinterpret_cast<unsigned char const*>(static_cast<char const*>(s_data_3)), static_cast<std::uint32_t>(std::size(s_data_3) - 1));
	if(adler_crc != s_checksum)
	{
		return false;
	}
	return true;
}

bool test_adler32_combine(unique_library const& zlib)
{
	auto const adler32_proc = get_proc_address(zlib, adler32, 12);
	auto const adler32_fn = reinterpret_cast<std::uint32_t(__stdcall*)(std::uint32_t, unsigned char const*, std::uint32_t)>(adler32_proc);
	auto const adler32_combine_proc = get_proc_address(zlib, adler32_combine, 12);
	auto const adler32_combine_fn = reinterpret_cast<std::uint32_t(__stdcall*)(std::uint32_t, std::uint32_t, std::ptrdiff_t)>(adler32_combine_proc);
	if(!adler32_combine_fn)
	{
		return false;
	}
	auto const adler_inital = adler32_fn(0, nullptr, 0);
	auto const adler1_crc = adler32_fn(adler_inital, reinterpret_cast<unsigned char const*>(static_cast<char const*>(s_data_1)), static_cast<std::uint32_t>(std::size(s_data_1) - 1));
	auto const adler2_crc = adler32_fn(adler_inital, reinterpret_cast<unsigned char const*>(static_cast<char const*>(s_data_2)), static_cast<std::uint32_t>(std::size(s_data_2) - 1));
	auto const adler12_crc = adler32_combine_fn(adler1_crc, adler2_crc, static_cast<unsigned int>(std::size(s_data_2) - 1));
	auto const adler3_crc = adler32_fn(adler_inital, reinterpret_cast<unsigned char const*>(static_cast<char const*>(s_data_3)), static_cast<std::uint32_t>(std::size(s_data_3) - 1));
	auto const adler123_crc = adler32_combine_fn(adler12_crc, adler3_crc, static_cast<unsigned int>(std::size(s_data_3) - 1));
	if(adler123_crc != s_checksum)
	{
		return false;
	}
	return true;
}

bool test_adler32z(unique_library const& zlib)
{
	auto const adler32_z_proc = get_proc_address(zlib, adler32_z, 12);
	auto const adler32_z_fn = reinterpret_cast<std::uint32_t(__stdcall*)(std::uint32_t, unsigned char const*, std::size_t)>(adler32_z_proc);
	if(!adler32_z_fn)
	{
		return false;
	}
	auto adlerz_crc = adler32_z_fn(0, nullptr, 0);
	adlerz_crc = adler32_z_fn(adlerz_crc, reinterpret_cast<unsigned char const*>(static_cast<char const*>(s_data_1)), static_cast<std::uint32_t>(std::size(s_data_1) - 1));
	adlerz_crc = adler32_z_fn(adlerz_crc, reinterpret_cast<unsigned char const*>(static_cast<char const*>(s_data_2)), static_cast<std::uint32_t>(std::size(s_data_2) - 1));
	adlerz_crc = adler32_z_fn(adlerz_crc, reinterpret_cast<unsigned char const*>(static_cast<char const*>(s_data_3)), static_cast<std::uint32_t>(std::size(s_data_3) - 1));
	if(adlerz_crc != s_checksum)
	{
		return false;
	}
	return true;
}

bool test_compress(unique_library const& zlib)
{
	char buff[1024];
	std::uint32_t buff_len = static_cast<std::uint32_t>(std::size(buff));
	auto const compress_proc = get_proc_address(zlib, compress, 16);
	auto const compress_fn = reinterpret_cast<std::int32_t(__stdcall*)(unsigned char*, std::uint32_t*, unsigned char const*, std::uint32_t)>(compress_proc);
	if(!compress_fn)
	{
		return false;
	}
	auto const compressed = compress_fn(reinterpret_cast<unsigned char*>(buff), &buff_len, reinterpret_cast<unsigned char const*>(static_cast<char const*>(s_long_data)), static_cast<std::uint32_t>(std::size(s_long_data) - 1));
	if(compressed != 0)
	{
		return false;
	}
	if(buff_len >= static_cast<std::uint32_t>(std::size(s_long_data) - 1))
	{
		return false;
	}
	return true;
}

bool test_compress2(unique_library const& zlib)
{
	char buff[1024];
	std::uint32_t buff_len = static_cast<std::uint32_t>(std::size(buff));
	auto const compress2_proc = get_proc_address(zlib, compress2, 20);
	auto const compress2_fn = reinterpret_cast<std::int32_t(__stdcall*)(unsigned char*, std::uint32_t*, unsigned char const*, std::uint32_t, std::int32_t)>(compress2_proc);
	if(!compress2_fn)
	{
		return false;
	}
	auto const compressed = compress2_fn(reinterpret_cast<unsigned char*>(buff), &buff_len, reinterpret_cast<unsigned char const*>(static_cast<char const*>(s_long_data)), static_cast<std::uint32_t>(std::size(s_long_data) - 1), 9);
	if(compressed != 0)
	{
		return false;
	}
	if(buff_len >= static_cast<std::uint32_t>(std::size(s_long_data) - 1))
	{
		return false;
	}
	return true;
}

bool test_compressBound(unique_library const& zlib)
{
	auto const compressBound_proc = get_proc_address(zlib, compressBound, 4);
	auto const compressBound_fn = reinterpret_cast<std::uint32_t(__stdcall*)(std::uint32_t)>(compressBound_proc);
	if(!compressBound_fn)
	{
		return false;
	}
	auto const bound = compressBound_fn(32 * 1024);
	if(bound <= 32 * 1024)
	{
		return false;
	}
	return true;
}
