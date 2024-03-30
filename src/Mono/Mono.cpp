#include <Windows.h>
#include <cstdint>
#include <iostream>
#include "Mono.hpp"

#define PROC_FUNC_CHECK(type, name, func) type name = func; if (name == nullptr) { std::cout << "failed to get " # name << '\n'; return nullptr; }

Mono::Mono()
	: m_isLoaded(false)
{
	if (!init())
		std::cout << "Couldn't create Mono instance!" << '\n';
	else
		m_isLoaded = true;
}

bool Mono::init()
{
	m_hMono = GetModuleHandleA("mono-2.0-bdwgc.dll");
	if (m_hMono == NULL)
		return false;

	m_mono_assembly_get_image = reinterpret_cast<mono_assembly_get_image_t>(GetProcAddress(m_hMono, "mono_assembly_get_image"));
	m_mono_image_loaded = reinterpret_cast<mono_image_loaded_t>(GetProcAddress(m_hMono, "mono_image_loaded"));
	m_mono_class_from_name = reinterpret_cast<mono_class_from_name_t>(GetProcAddress(m_hMono, "mono_class_from_name"));
	m_mono_class_get_method_from_name = reinterpret_cast<mono_class_get_method_from_name_t>(GetProcAddress(m_hMono, "mono_class_get_method_from_name"));
	m_mono_compile_method = reinterpret_cast<mono_compile_method_t>(GetProcAddress(m_hMono, "mono_compile_method"));
	m_mono_runtime_invoke = reinterpret_cast<mono_runtime_invoke_t>(GetProcAddress(m_hMono, "mono_runtime_invoke"));

	m_mono_class_get_field_from_name = reinterpret_cast<mono_class_get_field_from_name_t>(GetProcAddress(m_hMono, "mono_class_get_field_from_name"));
	m_mono_field_get_value = reinterpret_cast<mono_field_get_value_t>(GetProcAddress(m_hMono, "mono_field_get_value"));
	m_mono_field_set_value = reinterpret_cast<mono_field_set_value_t>(GetProcAddress(m_hMono, "mono_field_set_value"));
	m_mono_method_get_class = reinterpret_cast<mono_method_get_class_t>(GetProcAddress(m_hMono, "mono_method_get_class"));
	m_mono_class_vtable = reinterpret_cast<mono_class_vtable_t>(GetProcAddress(m_hMono, "mono_class_vtable"));
	m_mono_vtable_get_static_field_data = reinterpret_cast<mono_vtable_get_static_field_data_t>(GetProcAddress(m_hMono, "mono_vtable_get_static_field_data"));
	m_mono_field_get_offset = reinterpret_cast<mono_field_get_offset_t>(GetProcAddress(m_hMono, "mono_field_get_offset"));

	// Attach thread to prevent crashes
	m_mono_thread_attach = reinterpret_cast<mono_thread_attach_t>(GetProcAddress(m_hMono, "mono_thread_attach"));
	m_mono_get_root_domain = reinterpret_cast<mono_get_root_domain_t>(GetProcAddress(m_hMono, "mono_get_root_domain"));
	m_mono_thread_attach(m_mono_get_root_domain());

	return true;
}


Mono::MonoClassProxy Mono::getClass(const char* className, const char* assemblyName, const char* nameSpace)
{
	PROC_FUNC_CHECK(MonoImage*, pImage, m_mono_image_loaded(assemblyName));

	PROC_FUNC_CHECK(MonoClass*, pClass, m_mono_class_from_name(pImage, nameSpace, className));

	return pClass;
}

Mono::MonoClassProxy Mono::getClassFromMethod(MonoMethod* method)
{
	PROC_FUNC_CHECK(MonoClass*, pClass, m_mono_method_get_class(method));

	return pClass;
}

MonoObject* Mono::invoke(MonoMethod* method, void* obj, void** params)
{
	//? is this necessary for every invoke() call?
	m_mono_thread_attach(m_mono_get_root_domain());

	MonoObject* exc;
	return m_mono_runtime_invoke(method, obj, params, &exc);
}


MonoMethod* Mono::MonoClassProxy::getMethod(const char* methodName, int param_count)
{
	PROC_FUNC_CHECK(MonoMethod*, pMethod, Mono::Instance().m_mono_class_get_method_from_name(m_class, methodName, param_count));

	return pMethod;
}

void* Mono::MonoClassProxy::getCompiledMethod(const char* methodName, int param_count)
{
	PROC_FUNC_CHECK(MonoMethod*, pMethod, getMethod(methodName, param_count));

	PROC_FUNC_CHECK(void*, pCompiledMethod, Mono::Instance().m_mono_compile_method(pMethod));

	return pCompiledMethod;
}

MonoClassField* Mono::MonoClassProxy::getField(const char* fieldName)
{
	PROC_FUNC_CHECK(MonoClassField*, pField, Mono::Instance().m_mono_class_get_field_from_name(m_class, fieldName));

	return pField;
}

std::uint32_t Mono::MonoClassProxy::getFieldOffset(MonoClassField* field)
{
	return Mono::Instance().m_mono_field_get_offset(field);
}

void Mono::MonoClassProxy::getFieldValue(MonoObject* instance, MonoClassField* field, void* value)
{
	Mono::Instance().m_mono_field_get_value(instance, field, value);
}

void Mono::MonoClassProxy::setFieldValue(MonoObject* obj, MonoClassField* field, void* value)
{
	Mono::Instance().m_mono_field_set_value(obj, field, value);
}

MonoVTable* Mono::MonoClassProxy::getVTable()
{
	const auto& instance = Mono::Instance();

	PROC_FUNC_CHECK(MonoDomain*, pDomain, instance.m_mono_get_root_domain());

	return instance.m_mono_class_vtable(pDomain, m_class);
}

void* Mono::MonoClassProxy::getStaticFieldData()
{
	PROC_FUNC_CHECK(MonoVTable*, pVTable, getVTable());

	return Mono::Instance().m_mono_vtable_get_static_field_data(pVTable);
}

void* Mono::MonoClassProxy::getStaticFieldData(MonoVTable* pVTable)
{
	return Mono::Instance().m_mono_vtable_get_static_field_data(pVTable);
}

void* Mono::MonoClassProxy::getStaticFieldValue(const char* fieldName)
{
	PROC_FUNC_CHECK(MonoClassField*, pField, getField(fieldName));

	PROC_FUNC_CHECK(void*, pFieldData, getStaticFieldData());

	// addr
	std::uint32_t uFieldOffset = getFieldOffset(pField);

	return reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(pFieldData) + uFieldOffset);
}
