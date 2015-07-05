#include "Preferences.h"

#ifdef OPENWAR_USE_XCODE_FRAMEWORKS
#import <Foundation/Foundation.h>
#endif


#ifdef __ANDROID__

#include <codecvt>
#include <cstring>
#include <locale>
#include <jni.h>

extern "C" JNIEnv *Android_JNI_GetEnv(void);

static std::string ConvertFromJavaString(JNIEnv* env, jstring value)
{
	const jchar* data = env->GetStringChars(value, 0);
	jsize size = env->GetStringLength(value);

	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv(".");
	std::string result = conv.to_bytes((const char16_t*)data, (const char16_t*)data + size);

	env->ReleaseStringChars(value, data);

	return result;
}

static jstring ConvertToJavaString(JNIEnv* env, const char* value)
{
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv(".");
	std::u16string result = conv.from_bytes(value, value + std::strlen(value));

	return env->NewString((const jchar*)result.data(), (jsize)result.size());
}

#endif


std::string Preferences::GetString(const char* key)
{
#ifdef OPENWAR_USE_XCODE_FRAMEWORKS
	NSUserDefaults* userDefaults = [NSUserDefaults standardUserDefaults];
	NSString* value = [userDefaults stringForKey:[NSString stringWithUTF8String:key]];
	return std::string{value.UTF8String ?: ""};
#endif

#ifdef __ANDROID__

	JNIEnv* env = Android_JNI_GetEnv();
	if (!env)
		return "";

	jclass clazz = env->FindClass("org/openwar/Preferences");
	if (!clazz)
		return "";

	jmethodID method = env->GetStaticMethodID(clazz, "getString", "(Ljava/lang/String;)Ljava/lang/String;");
	if (!method)
		return "";

	jstring param = ConvertToJavaString(env, key);
	jstring value = static_cast<jstring>(env->CallStaticObjectMethod(clazz, method, param));

	env->DeleteLocalRef(clazz);

	std::string result = ConvertFromJavaString(env, value);

	env->DeleteLocalRef(param);
	env->DeleteLocalRef(value);

	return result;

#endif

	return std::string{};
};


void Preferences::SetString(const char* key, const char* value)
{
#ifdef OPENWAR_USE_XCODE_FRAMEWORKS
	NSUserDefaults* userDefaults = [NSUserDefaults standardUserDefaults];
	[userDefaults setObject:[NSString stringWithUTF8String:value] forKey:[NSString stringWithUTF8String:key]];
#endif

#ifdef __ANDROID__

	JNIEnv* env = Android_JNI_GetEnv();
	if (!env)
		return;

	jclass clazz = env->FindClass("org/openwar/Preferences");
	if (!clazz)
		return;

	jmethodID method = env->GetStaticMethodID(clazz, "setString", "(Ljava/lang/String;Ljava/lang/String;)V");
	if (!method)
		return;

	jstring param1 = ConvertToJavaString(env, key);
	jstring param2 = ConvertToJavaString(env, value);
	env->CallStaticVoidMethod(clazz, method, param1, param2);

	env->DeleteLocalRef(clazz);
	env->DeleteLocalRef(param1);
	env->DeleteLocalRef(param1);

#endif
}


bool Preferences::GetBoolean(const char* key, bool defaultValue)
{
#ifdef OPENWAR_USE_XCODE_FRAMEWORKS
	NSUserDefaults* userDefaults = [NSUserDefaults standardUserDefaults];
	NSString* string = [NSString stringWithUTF8String:key];
	if ([userDefaults objectForKey:string] == nil)
		return defaultValue;
	return [userDefaults boolForKey:string];
#endif

	return false;
}


void Preferences::SetBoolean(const char* key, bool value)
{
#ifdef OPENWAR_USE_XCODE_FRAMEWORKS
	NSUserDefaults* userDefaults = [NSUserDefaults standardUserDefaults];
	[userDefaults setBool:value forKey:[NSString stringWithUTF8String:key]];
#endif
}
