#pragma once
#include <cstdint>
#include <Windows.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/object.h>

// https://www.unknowncheats.me/forum/unity/603179-hacking-mono-games.html
class Mono
{
private:
	struct MonoClassProxy
	{
	private:
		MonoClass* m_class;

	public:
		MonoClassProxy(std::nullptr_t)
			: m_class(nullptr)
		{}

		MonoClassProxy(MonoClass* monoClass)
			: m_class(monoClass)
		{};

		// TODO: MonoMethodProxy?
		MonoMethod* getMethod(const char* methodName, int param_count = 0);
		void* getCompiledMethod(const char* methodName, int param_count = 0);

		MonoClassField* getField(const char* fieldName);
		void getFieldValue(MonoObject* instance, MonoClassField* field, void* out);
		void setFieldValue(MonoObject* obj, MonoClassField* field, void* value);
		std::uint32_t getFieldOffset(MonoClassField* field);

		void* getStaticFieldData();
		void* getStaticFieldData(MonoVTable* pVTable);
		void* getStaticFieldValue(const char* fieldName);

		MonoVTable* getVTable();


		operator bool()
		{
			return m_class != nullptr;
		}
	};

	friend struct MonoClassProxy;

public:
	inline static Mono& Instance()
	{
		static Mono instance;

		return instance;
	}

private:
	Mono();
	bool init();

public:
	Mono(const Mono&) = delete;
	void operator=(const Mono&) = delete;

public:
	MonoClassProxy getClass(const char* className, const char* assemblyName = "Assembly-CSharp", const char* nameSpace = "");
	MonoClassProxy getClassFromMethod(MonoMethod* method);

	MonoObject* invoke(MonoMethod* method, void* obj, void** params);


	bool isLoaded() { return m_isLoaded; }

private:
	HMODULE m_hMono;
	bool m_isLoaded;

	using mono_thread_attach_t = MonoThread*(*)(MonoDomain* domain);
	using mono_get_root_domain_t = MonoDomain*(*)();
	using mono_image_loaded_t = MonoImage*(*)(const char* name);
	using mono_assembly_get_image_t = MonoImage*(*)(MonoAssembly* assembly);
	using mono_class_from_name_t = MonoClass*(*)(MonoImage* image, const char* name_space, const char* name);
	using mono_class_get_method_from_name_t = MonoMethod*(*)(MonoClass* klass, const char* name, int param_count);
	using mono_compile_method_t = void* (*)(MonoMethod* method);
	using mono_runtime_invoke_t = MonoObject*(*)(MonoMethod* method, void* obj, void** params, MonoObject** exc);

	using mono_class_get_field_from_name_t = MonoClassField*(*)(MonoClass* klass, const char* name);
	using mono_field_get_value_t = void* (*)(void* obj, MonoClassField* field, void* value);
	using mono_field_set_value_t = void(*)(MonoObject* obj, MonoClassField* field, void* value);
	using mono_method_get_class_t = MonoClass*(*)(MonoMethod* method);
	using mono_class_vtable_t = MonoVTable*(*)(MonoDomain* domain, MonoClass* klass);
	using mono_vtable_get_static_field_data_t = void* (*)(MonoVTable* vt);
	using mono_field_get_offset_t = uint32_t(*)(MonoClassField* field);

	mono_image_loaded_t m_mono_image_loaded;
	mono_assembly_get_image_t m_mono_assembly_get_image;
	mono_class_from_name_t m_mono_class_from_name;
	mono_class_get_method_from_name_t m_mono_class_get_method_from_name;
	mono_compile_method_t m_mono_compile_method;
	mono_runtime_invoke_t m_mono_runtime_invoke;
	mono_class_get_field_from_name_t m_mono_class_get_field_from_name;
	mono_field_get_value_t m_mono_field_get_value;
	mono_field_set_value_t m_mono_field_set_value;
	mono_method_get_class_t m_mono_method_get_class;
	mono_class_vtable_t m_mono_class_vtable;
	mono_vtable_get_static_field_data_t m_mono_vtable_get_static_field_data;
	mono_field_get_offset_t m_mono_field_get_offset;
	mono_thread_attach_t m_mono_thread_attach;
	mono_get_root_domain_t m_mono_get_root_domain;
};
