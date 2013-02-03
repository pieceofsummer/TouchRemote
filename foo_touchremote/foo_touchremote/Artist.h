#pragma once

using namespace System;
using namespace TouchRemote::Interfaces;

namespace foo_touchremote
{

	public ref class Artist : public IArtist
	{
	public:
		Artist(IMediaLibrary^ library, String^ name);

		virtual property int Id
		{
			int get();
		}

		virtual property __int64 PersistentId
		{
			__int64 get();
		}

		virtual bool Equals(IArtist ^other);

		virtual int CompareTo(IArtist ^other);

		virtual property String^ Name
		{
			String^ get();
		}

		virtual String^ ToString() override;
		virtual bool Equals(Object^ other) override;
		virtual int GetHashCode() override;

	private:
		IMediaLibrary^ m_library;
		String^ m_name;
	};
}