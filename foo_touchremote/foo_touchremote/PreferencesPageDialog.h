#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace TouchRemote::Core::Pairing;

#include "PreferencesPageInstance.h"

#pragma managed

#include "Utils.h"
#include "Settings.h"
#include "ManagedHost.h"
#include "PairingDialog.h"

namespace foo_touchremote 
{
	/// <summary>
	/// Summary for PreferencesPageDialog
	/// </summary>
	public ref class PreferencesPageDialog : public System::Windows::Forms::UserControl
	{
	private:
		IntPtr m_parent;
		int m_state;
		preferences_page_callback *m_callback;
        foobar::PreferencesPageInstance *m_pageInstance;



	private: System::Windows::Forms::TextBox^  displayName;
	private: System::Windows::Forms::ListBox^  devicesList;
	private: System::Windows::Forms::Button^  deleteAllDevicesButton;
	private: System::Windows::Forms::Button^  deleteSelectedDeviceButton;
	private: System::Windows::Forms::Button^  addNewDeviceButton;

	private: System::Windows::Forms::TabPage^  generalPage;

	private: System::Windows::Forms::TabPage^  customizationPage;
	private: System::Windows::Forms::TextBox^  songTitleFormat;
	private: System::Windows::Forms::ErrorProvider^  titleformattingError;
	private: System::Windows::Forms::TextBox^  songGenreFormat;
	private: System::Windows::Forms::TextBox^  songComposerFormat;
	private: System::Windows::Forms::TextBox^  songAlbumFormat;
	private: System::Windows::Forms::TextBox^  songAlbumArtistFormat;
	private: System::Windows::Forms::TextBox^  songArtistFormat;
	private: System::Windows::Forms::TextBox^  songRatingFormat;
	private: System::Windows::Forms::CheckBox^  checkBox1;
	private: System::Windows::Forms::PictureBox^  generalPageLogo;

	private: System::Windows::Forms::PictureBox^  customizationPageLogo;

	private: System::Windows::Forms::CheckBox^  allowGuestLoginsCheckBox;
	private: System::Windows::Forms::TabPage^  networkPage;
	private: System::Windows::Forms::NumericUpDown^  portNumber;

	private: System::Windows::Forms::CheckBox^  trafficCompressionCheckBox;
	private: System::Windows::Forms::GroupBox^  djGroup;
	private: System::Windows::Forms::CheckBox^  httpLoggingCheckBox;
    private: System::Windows::Forms::CheckBox^  stopOnPauseCheckBox;
    private: System::Windows::Forms::TabControl^  tabPages;

	private: System::Windows::Forms::PictureBox^  networkPageLogo;



	public:
		PreferencesPageDialog(foobar::PreferencesPageInstance *pageInstance, IntPtr parent, preferences_page_callback* callback)
		{
			m_parent = parent;
			m_callback = NULL;

			InitializeComponent();

			customizationPageLogo->Image = generalPageLogo->Image;
			networkPageLogo->Image = generalPageLogo->Image;

			LoadSettings();

            m_pageInstance = pageInstance;
			m_callback = callback;

            // workaround bug in tabcontrol
            tabPages->HandleCreated += gcnew EventHandler(this, &PreferencesPageDialog::ControlHandleCreated);
		}

		void Apply()
		{
			settings::DisplayName = ToUtf8String(displayName->Text);
			settings::Port = (t_uint16)portNumber->Value;
			settings::HttpLogging = httpLoggingCheckBox->Checked;

			settings::SongTitleFormat = ToUtf8String(songTitleFormat->Text);
			settings::SongAlbumFormat = ToUtf8String(songAlbumFormat->Text);
			settings::SongAlbumArtistFormat = ToUtf8String(songAlbumArtistFormat->Text);
			settings::SongArtistFormat = ToUtf8String(songArtistFormat->Text);
			settings::SongComposerFormat = ToUtf8String(songComposerFormat->Text);
			settings::SongGenreFormat = ToUtf8String(songGenreFormat->Text);
			settings::SongRatingFormat = ToUtf8String(songRatingFormat->Text);

			settings::CompressNetworkTraffic = trafficCompressionCheckBox->Checked;
			settings::AllowGuestLogins = allowGuestLoginsCheckBox->Checked;
            settings::StopOnPause = stopOnPauseCheckBox->Checked;
		}
		
		void LoadSettings()
		{
			displayName->Text = FromUtf8String(settings::DisplayName);
			portNumber->Value = Convert::ToDecimal((t_uint16)settings::Port);
			httpLoggingCheckBox->Checked = settings::HttpLogging;

			songTitleFormat->Text = FromUtf8String(settings::SongTitleFormat);
			songAlbumFormat->Text = FromUtf8String(settings::SongAlbumFormat);
			songAlbumArtistFormat->Text = FromUtf8String(settings::SongAlbumArtistFormat);
			songArtistFormat->Text = FromUtf8String(settings::SongArtistFormat);
			songComposerFormat->Text = FromUtf8String(settings::SongComposerFormat);
			songGenreFormat->Text = FromUtf8String(settings::SongGenreFormat);
			songRatingFormat->Text = FromUtf8String(settings::SongRatingFormat);
			
			trafficCompressionCheckBox->Checked = settings::CompressNetworkTraffic;
			allowGuestLoginsCheckBox->Checked = settings::AllowGuestLogins;
            stopOnPauseCheckBox->Checked = settings::StopOnPause;

			LoadDeviceList();
			UpdateDeviceState();
			State = 0;
		}

		void Reset()
		{
			displayName->Text = FromUtf8String(settings::defaults::DisplayName);
			portNumber->Value = Convert::ToDecimal((t_uint16)settings::defaults::Port);
			httpLoggingCheckBox->Checked = settings::defaults::HttpLogging;

			songTitleFormat->Text = FromUtf8String(settings::defaults::SongTitleFormat);
			songAlbumFormat->Text = FromUtf8String(settings::defaults::SongAlbumFormat);
			songAlbumArtistFormat->Text = FromUtf8String(settings::defaults::SongAlbumArtistFormat);
			songArtistFormat->Text = FromUtf8String(settings::defaults::SongArtistFormat);
			songComposerFormat->Text = FromUtf8String(settings::defaults::SongComposerFormat);
			songGenreFormat->Text = FromUtf8String(settings::defaults::SongGenreFormat);
			songRatingFormat->Text = FromUtf8String(settings::defaults::SongRatingFormat);

			trafficCompressionCheckBox->Checked = settings::defaults::CompressNetworkTraffic;
			allowGuestLoginsCheckBox->Checked = settings::defaults::AllowGuestLogins;
            stopOnPauseCheckBox->Checked = settings::defaults::StopOnPause;
		}
		
		property int State
		{
			int get() 
			{ 
				return m_state | preferences_state::resettable; 
			}
			void set(int value) 
			{ 
				m_state = value; 
				if (m_callback)
					m_callback->on_state_changed();
			}
		}

        void SetPageNull()
        {
            m_pageInstance = NULL;
        }

        void DestroyWindowHandle()
        {
            Parent = nullptr;
            if (!IsDisposed)
            {
                ReleaseCapture();
                delete this;
            }
        }

	protected:

		~PreferencesPageDialog()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::ComponentModel::IContainer^  components;
	protected: 

		virtual property System::Windows::Forms::CreateParams^ CreateParams
		{
			System::Windows::Forms::CreateParams^ get() override
			{
				System::Windows::Forms::CreateParams^ p = __super::CreateParams;

				p->Style |= WS_CHILD | WS_VISIBLE;
				p->Parent = m_parent;

				return p;
			}
		}

        virtual void OnHandleDestroyed(System::EventArgs ^e) override
        {
            __super::OnHandleDestroyed(e);
            if (m_pageInstance != NULL)
                m_pageInstance->SetDestroyInProgress();
        }
	protected: 
		
	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


		#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
            this->components = (gcnew System::ComponentModel::Container());
            System::Windows::Forms::GroupBox^  devicesGroup;
            System::Windows::Forms::GroupBox^  displayNameGroup;
            System::Windows::Forms::GroupBox^  formatStringsGroup;
            System::Windows::Forms::Label^  label10;
            System::Windows::Forms::Label^  label9;
            System::Windows::Forms::Label^  label8;
            System::Windows::Forms::Label^  label7;
            System::Windows::Forms::Label^  label6;
            System::Windows::Forms::Label^  label5;
            System::Windows::Forms::Label^  label4;
            System::Windows::Forms::GroupBox^  debugGroup;
            System::Windows::Forms::Label^  label2;
            System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(PreferencesPageDialog::typeid));
            this->deleteAllDevicesButton = (gcnew System::Windows::Forms::Button());
            this->deleteSelectedDeviceButton = (gcnew System::Windows::Forms::Button());
            this->addNewDeviceButton = (gcnew System::Windows::Forms::Button());
            this->devicesList = (gcnew System::Windows::Forms::ListBox());
            this->displayName = (gcnew System::Windows::Forms::TextBox());
            this->songRatingFormat = (gcnew System::Windows::Forms::TextBox());
            this->songGenreFormat = (gcnew System::Windows::Forms::TextBox());
            this->songComposerFormat = (gcnew System::Windows::Forms::TextBox());
            this->songAlbumFormat = (gcnew System::Windows::Forms::TextBox());
            this->songAlbumArtistFormat = (gcnew System::Windows::Forms::TextBox());
            this->songArtistFormat = (gcnew System::Windows::Forms::TextBox());
            this->songTitleFormat = (gcnew System::Windows::Forms::TextBox());
            this->httpLoggingCheckBox = (gcnew System::Windows::Forms::CheckBox());
            this->tabPages = (gcnew System::Windows::Forms::TabControl());
            this->generalPage = (gcnew System::Windows::Forms::TabPage());
            this->djGroup = (gcnew System::Windows::Forms::GroupBox());
            this->allowGuestLoginsCheckBox = (gcnew System::Windows::Forms::CheckBox());
            this->generalPageLogo = (gcnew System::Windows::Forms::PictureBox());
            this->customizationPage = (gcnew System::Windows::Forms::TabPage());
            this->stopOnPauseCheckBox = (gcnew System::Windows::Forms::CheckBox());
            this->customizationPageLogo = (gcnew System::Windows::Forms::PictureBox());
            this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
            this->networkPage = (gcnew System::Windows::Forms::TabPage());
            this->trafficCompressionCheckBox = (gcnew System::Windows::Forms::CheckBox());
            this->portNumber = (gcnew System::Windows::Forms::NumericUpDown());
            this->networkPageLogo = (gcnew System::Windows::Forms::PictureBox());
            this->titleformattingError = (gcnew System::Windows::Forms::ErrorProvider(this->components));
            devicesGroup = (gcnew System::Windows::Forms::GroupBox());
            displayNameGroup = (gcnew System::Windows::Forms::GroupBox());
            formatStringsGroup = (gcnew System::Windows::Forms::GroupBox());
            label10 = (gcnew System::Windows::Forms::Label());
            label9 = (gcnew System::Windows::Forms::Label());
            label8 = (gcnew System::Windows::Forms::Label());
            label7 = (gcnew System::Windows::Forms::Label());
            label6 = (gcnew System::Windows::Forms::Label());
            label5 = (gcnew System::Windows::Forms::Label());
            label4 = (gcnew System::Windows::Forms::Label());
            debugGroup = (gcnew System::Windows::Forms::GroupBox());
            label2 = (gcnew System::Windows::Forms::Label());
            devicesGroup->SuspendLayout();
            displayNameGroup->SuspendLayout();
            formatStringsGroup->SuspendLayout();
            debugGroup->SuspendLayout();
            this->tabPages->SuspendLayout();
            this->generalPage->SuspendLayout();
            this->djGroup->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->generalPageLogo))->BeginInit();
            this->customizationPage->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->customizationPageLogo))->BeginInit();
            this->networkPage->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->portNumber))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->networkPageLogo))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->titleformattingError))->BeginInit();
            this->SuspendLayout();
            // 
            // devicesGroup
            // 
            devicesGroup->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
                | System::Windows::Forms::AnchorStyles::Right));
            devicesGroup->Controls->Add(this->deleteAllDevicesButton);
            devicesGroup->Controls->Add(this->deleteSelectedDeviceButton);
            devicesGroup->Controls->Add(this->addNewDeviceButton);
            devicesGroup->Controls->Add(this->devicesList);
            devicesGroup->Location = System::Drawing::Point(6, 75);
            devicesGroup->Name = L"devicesGroup";
            devicesGroup->Size = System::Drawing::Size(510, 153);
            devicesGroup->TabIndex = 5;
            devicesGroup->TabStop = false;
            devicesGroup->Text = L"Devices";
            // 
            // deleteAllDevicesButton
            // 
            this->deleteAllDevicesButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
            this->deleteAllDevicesButton->Location = System::Drawing::Point(360, 120);
            this->deleteAllDevicesButton->Name = L"deleteAllDevicesButton";
            this->deleteAllDevicesButton->Size = System::Drawing::Size(144, 27);
            this->deleteAllDevicesButton->TabIndex = 3;
            this->deleteAllDevicesButton->Text = L"Delete all devices";
            this->deleteAllDevicesButton->UseVisualStyleBackColor = true;
            this->deleteAllDevicesButton->Click += gcnew System::EventHandler(this, &PreferencesPageDialog::deleteAllDevicesButton_Click);
            // 
            // deleteSelectedDeviceButton
            // 
            this->deleteSelectedDeviceButton->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
            this->deleteSelectedDeviceButton->Location = System::Drawing::Point(182, 120);
            this->deleteSelectedDeviceButton->Name = L"deleteSelectedDeviceButton";
            this->deleteSelectedDeviceButton->Size = System::Drawing::Size(144, 27);
            this->deleteSelectedDeviceButton->TabIndex = 2;
            this->deleteSelectedDeviceButton->Text = L"Delete selected device";
            this->deleteSelectedDeviceButton->UseVisualStyleBackColor = true;
            this->deleteSelectedDeviceButton->Click += gcnew System::EventHandler(this, &PreferencesPageDialog::deleteSelectedDeviceButton_Click);
            // 
            // addNewDeviceButton
            // 
            this->addNewDeviceButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
            this->addNewDeviceButton->Location = System::Drawing::Point(6, 120);
            this->addNewDeviceButton->Name = L"addNewDeviceButton";
            this->addNewDeviceButton->Size = System::Drawing::Size(144, 27);
            this->addNewDeviceButton->TabIndex = 1;
            this->addNewDeviceButton->Text = L"Add a new device...";
            this->addNewDeviceButton->UseVisualStyleBackColor = true;
            this->addNewDeviceButton->Click += gcnew System::EventHandler(this, &PreferencesPageDialog::addNewDeviceButton_Click);
            // 
            // devicesList
            // 
            this->devicesList->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
                | System::Windows::Forms::AnchorStyles::Left) 
                | System::Windows::Forms::AnchorStyles::Right));
            this->devicesList->DisplayMember = L"Name";
            this->devicesList->FormattingEnabled = true;
            this->devicesList->Location = System::Drawing::Point(6, 19);
            this->devicesList->Name = L"devicesList";
            this->devicesList->Size = System::Drawing::Size(498, 95);
            this->devicesList->TabIndex = 0;
            this->devicesList->SelectedIndexChanged += gcnew System::EventHandler(this, &PreferencesPageDialog::devicesList_SelectedIndexChanged);
            // 
            // displayNameGroup
            // 
            displayNameGroup->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
                | System::Windows::Forms::AnchorStyles::Right));
            displayNameGroup->Controls->Add(this->displayName);
            displayNameGroup->Location = System::Drawing::Point(6, 6);
            displayNameGroup->Name = L"displayNameGroup";
            displayNameGroup->Size = System::Drawing::Size(510, 52);
            displayNameGroup->TabIndex = 9;
            displayNameGroup->TabStop = false;
            displayNameGroup->Text = L"Display name";
            // 
            // displayName
            // 
            this->displayName->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
                | System::Windows::Forms::AnchorStyles::Right));
            this->displayName->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
                static_cast<System::Byte>(204)));
            this->displayName->Location = System::Drawing::Point(6, 19);
            this->displayName->Name = L"displayName";
            this->displayName->Size = System::Drawing::Size(498, 23);
            this->displayName->TabIndex = 4;
            this->displayName->Text = L"foobar 2000";
            this->displayName->TextChanged += gcnew System::EventHandler(this, &PreferencesPageDialog::displayName_TextChanged);
            // 
            // formatStringsGroup
            // 
            formatStringsGroup->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
                | System::Windows::Forms::AnchorStyles::Right));
            formatStringsGroup->Controls->Add(label10);
            formatStringsGroup->Controls->Add(label9);
            formatStringsGroup->Controls->Add(label8);
            formatStringsGroup->Controls->Add(label7);
            formatStringsGroup->Controls->Add(label6);
            formatStringsGroup->Controls->Add(label5);
            formatStringsGroup->Controls->Add(label4);
            formatStringsGroup->Controls->Add(this->songRatingFormat);
            formatStringsGroup->Controls->Add(this->songGenreFormat);
            formatStringsGroup->Controls->Add(this->songComposerFormat);
            formatStringsGroup->Controls->Add(this->songAlbumFormat);
            formatStringsGroup->Controls->Add(this->songAlbumArtistFormat);
            formatStringsGroup->Controls->Add(this->songArtistFormat);
            formatStringsGroup->Controls->Add(this->songTitleFormat);
            formatStringsGroup->Location = System::Drawing::Point(6, 6);
            formatStringsGroup->Name = L"formatStringsGroup";
            formatStringsGroup->Size = System::Drawing::Size(510, 207);
            formatStringsGroup->TabIndex = 0;
            formatStringsGroup->TabStop = false;
            formatStringsGroup->Text = L"Format strings";
            // 
            // label10
            // 
            label10->Location = System::Drawing::Point(10, 175);
            label10->Name = L"label10";
            label10->Size = System::Drawing::Size(80, 20);
            label10->TabIndex = 9;
            label10->Text = L"Rating:";
            label10->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
            label10->UseMnemonic = false;
            // 
            // label9
            // 
            label9->Location = System::Drawing::Point(10, 149);
            label9->Name = L"label9";
            label9->Size = System::Drawing::Size(80, 20);
            label9->TabIndex = 9;
            label9->Text = L"Genre:";
            label9->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
            label9->UseMnemonic = false;
            // 
            // label8
            // 
            label8->Location = System::Drawing::Point(10, 123);
            label8->Name = L"label8";
            label8->Size = System::Drawing::Size(80, 20);
            label8->TabIndex = 9;
            label8->Text = L"Composer:";
            label8->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
            label8->UseMnemonic = false;
            // 
            // label7
            // 
            label7->Location = System::Drawing::Point(10, 97);
            label7->Name = L"label7";
            label7->Size = System::Drawing::Size(80, 20);
            label7->TabIndex = 9;
            label7->Text = L"Song Artist:";
            label7->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
            label7->UseMnemonic = false;
            // 
            // label6
            // 
            label6->Location = System::Drawing::Point(10, 71);
            label6->Name = L"label6";
            label6->Size = System::Drawing::Size(80, 20);
            label6->TabIndex = 9;
            label6->Text = L"Album Artist:";
            label6->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
            label6->UseMnemonic = false;
            // 
            // label5
            // 
            label5->Location = System::Drawing::Point(10, 45);
            label5->Name = L"label5";
            label5->Size = System::Drawing::Size(80, 20);
            label5->TabIndex = 9;
            label5->Text = L"Album:";
            label5->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
            label5->UseMnemonic = false;
            // 
            // label4
            // 
            label4->Location = System::Drawing::Point(10, 19);
            label4->Name = L"label4";
            label4->Size = System::Drawing::Size(80, 20);
            label4->TabIndex = 8;
            label4->Text = L"Title:";
            label4->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
            label4->UseMnemonic = false;
            // 
            // songRatingFormat
            // 
            this->songRatingFormat->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
                | System::Windows::Forms::AnchorStyles::Right));
            this->titleformattingError->SetIconAlignment(this->songRatingFormat, System::Windows::Forms::ErrorIconAlignment::MiddleLeft);
            this->songRatingFormat->Location = System::Drawing::Point(96, 175);
            this->songRatingFormat->Name = L"songRatingFormat";
            this->songRatingFormat->Size = System::Drawing::Size(408, 20);
            this->songRatingFormat->TabIndex = 7;
            this->songRatingFormat->Text = L"$if2(%rating%,0)";
            this->songRatingFormat->TextChanged += gcnew System::EventHandler(this, &PreferencesPageDialog::TitleFormat_TextChanged);
            this->songRatingFormat->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &PreferencesPageDialog::TitleFormat_Validating);
            this->songRatingFormat->Validated += gcnew System::EventHandler(this, &PreferencesPageDialog::TitleFormat_Validated);
            // 
            // songGenreFormat
            // 
            this->songGenreFormat->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
                | System::Windows::Forms::AnchorStyles::Right));
            this->titleformattingError->SetIconAlignment(this->songGenreFormat, System::Windows::Forms::ErrorIconAlignment::MiddleLeft);
            this->songGenreFormat->Location = System::Drawing::Point(96, 149);
            this->songGenreFormat->Name = L"songGenreFormat";
            this->songGenreFormat->Size = System::Drawing::Size(408, 20);
            this->songGenreFormat->TabIndex = 6;
            this->songGenreFormat->Text = L"%genre%";
            this->songGenreFormat->TextChanged += gcnew System::EventHandler(this, &PreferencesPageDialog::TitleFormat_TextChanged);
            this->songGenreFormat->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &PreferencesPageDialog::TitleFormat_Validating);
            this->songGenreFormat->Validated += gcnew System::EventHandler(this, &PreferencesPageDialog::TitleFormat_Validated);
            // 
            // songComposerFormat
            // 
            this->songComposerFormat->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
                | System::Windows::Forms::AnchorStyles::Right));
            this->titleformattingError->SetIconAlignment(this->songComposerFormat, System::Windows::Forms::ErrorIconAlignment::MiddleLeft);
            this->songComposerFormat->Location = System::Drawing::Point(96, 123);
            this->songComposerFormat->Name = L"songComposerFormat";
            this->songComposerFormat->Size = System::Drawing::Size(408, 20);
            this->songComposerFormat->TabIndex = 5;
            this->songComposerFormat->Text = L"%composer%";
            this->songComposerFormat->TextChanged += gcnew System::EventHandler(this, &PreferencesPageDialog::TitleFormat_TextChanged);
            this->songComposerFormat->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &PreferencesPageDialog::TitleFormat_Validating);
            this->songComposerFormat->Validated += gcnew System::EventHandler(this, &PreferencesPageDialog::TitleFormat_Validated);
            // 
            // songAlbumFormat
            // 
            this->songAlbumFormat->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
                | System::Windows::Forms::AnchorStyles::Right));
            this->titleformattingError->SetIconAlignment(this->songAlbumFormat, System::Windows::Forms::ErrorIconAlignment::MiddleLeft);
            this->songAlbumFormat->Location = System::Drawing::Point(96, 45);
            this->songAlbumFormat->Name = L"songAlbumFormat";
            this->songAlbumFormat->Size = System::Drawing::Size(408, 20);
            this->songAlbumFormat->TabIndex = 4;
            this->songAlbumFormat->Text = L"%album%";
            this->songAlbumFormat->TextChanged += gcnew System::EventHandler(this, &PreferencesPageDialog::TitleFormat_TextChanged);
            this->songAlbumFormat->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &PreferencesPageDialog::TitleFormat_Validating);
            this->songAlbumFormat->Validated += gcnew System::EventHandler(this, &PreferencesPageDialog::TitleFormat_Validated);
            // 
            // songAlbumArtistFormat
            // 
            this->songAlbumArtistFormat->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
                | System::Windows::Forms::AnchorStyles::Right));
            this->titleformattingError->SetIconAlignment(this->songAlbumArtistFormat, System::Windows::Forms::ErrorIconAlignment::MiddleLeft);
            this->songAlbumArtistFormat->Location = System::Drawing::Point(96, 71);
            this->songAlbumArtistFormat->Name = L"songAlbumArtistFormat";
            this->songAlbumArtistFormat->Size = System::Drawing::Size(408, 20);
            this->songAlbumArtistFormat->TabIndex = 3;
            this->songAlbumArtistFormat->Text = L"$if2(%album artist%,%artist%)";
            this->songAlbumArtistFormat->TextChanged += gcnew System::EventHandler(this, &PreferencesPageDialog::TitleFormat_TextChanged);
            this->songAlbumArtistFormat->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &PreferencesPageDialog::TitleFormat_Validating);
            this->songAlbumArtistFormat->Validated += gcnew System::EventHandler(this, &PreferencesPageDialog::TitleFormat_Validated);
            // 
            // songArtistFormat
            // 
            this->songArtistFormat->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
                | System::Windows::Forms::AnchorStyles::Right));
            this->titleformattingError->SetIconAlignment(this->songArtistFormat, System::Windows::Forms::ErrorIconAlignment::MiddleLeft);
            this->songArtistFormat->Location = System::Drawing::Point(96, 97);
            this->songArtistFormat->Name = L"songArtistFormat";
            this->songArtistFormat->Size = System::Drawing::Size(408, 20);
            this->songArtistFormat->TabIndex = 2;
            this->songArtistFormat->Text = L"%artist%";
            this->songArtistFormat->TextChanged += gcnew System::EventHandler(this, &PreferencesPageDialog::TitleFormat_TextChanged);
            this->songArtistFormat->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &PreferencesPageDialog::TitleFormat_Validating);
            this->songArtistFormat->Validated += gcnew System::EventHandler(this, &PreferencesPageDialog::TitleFormat_Validated);
            // 
            // songTitleFormat
            // 
            this->songTitleFormat->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
                | System::Windows::Forms::AnchorStyles::Right));
            this->titleformattingError->SetIconAlignment(this->songTitleFormat, System::Windows::Forms::ErrorIconAlignment::MiddleLeft);
            this->songTitleFormat->Location = System::Drawing::Point(96, 19);
            this->songTitleFormat->Name = L"songTitleFormat";
            this->songTitleFormat->Size = System::Drawing::Size(408, 20);
            this->songTitleFormat->TabIndex = 1;
            this->songTitleFormat->Text = L"%title%";
            this->songTitleFormat->TextChanged += gcnew System::EventHandler(this, &PreferencesPageDialog::TitleFormat_TextChanged);
            this->songTitleFormat->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &PreferencesPageDialog::TitleFormat_Validating);
            this->songTitleFormat->Validated += gcnew System::EventHandler(this, &PreferencesPageDialog::TitleFormat_Validated);
            // 
            // debugGroup
            // 
            debugGroup->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
                | System::Windows::Forms::AnchorStyles::Right));
            debugGroup->Controls->Add(this->httpLoggingCheckBox);
            debugGroup->Location = System::Drawing::Point(6, 50);
            debugGroup->Name = L"debugGroup";
            debugGroup->Size = System::Drawing::Size(510, 43);
            debugGroup->TabIndex = 16;
            debugGroup->TabStop = false;
            debugGroup->Text = L"Debug";
            // 
            // httpLoggingCheckBox
            // 
            this->httpLoggingCheckBox->AutoSize = true;
            this->httpLoggingCheckBox->Location = System::Drawing::Point(6, 19);
            this->httpLoggingCheckBox->Name = L"httpLoggingCheckBox";
            this->httpLoggingCheckBox->Size = System::Drawing::Size(139, 17);
            this->httpLoggingCheckBox->TabIndex = 6;
            this->httpLoggingCheckBox->Text = L"Log requests to console";
            this->httpLoggingCheckBox->UseVisualStyleBackColor = true;
            this->httpLoggingCheckBox->CheckedChanged += gcnew System::EventHandler(this, &PreferencesPageDialog::httpLoggingCheckBox_CheckedChanged);
            // 
            // label2
            // 
            label2->AutoSize = true;
            label2->Location = System::Drawing::Point(9, 15);
            label2->Name = L"label2";
            label2->Size = System::Drawing::Size(29, 13);
            label2->TabIndex = 13;
            label2->Text = L"Port:";
            // 
            // tabPages
            // 
            this->tabPages->Controls->Add(this->generalPage);
            this->tabPages->Controls->Add(this->customizationPage);
            this->tabPages->Controls->Add(this->networkPage);
            this->tabPages->Dock = System::Windows::Forms::DockStyle::Fill;
            this->tabPages->Location = System::Drawing::Point(0, 0);
            this->tabPages->Name = L"tabPages";
            this->tabPages->SelectedIndex = 0;
            this->tabPages->Size = System::Drawing::Size(530, 466);
            this->tabPages->TabIndex = 8;
            // 
            // generalPage
            // 
            this->generalPage->Controls->Add(this->djGroup);
            this->generalPage->Controls->Add(displayNameGroup);
            this->generalPage->Controls->Add(this->generalPageLogo);
            this->generalPage->Controls->Add(devicesGroup);
            this->generalPage->Location = System::Drawing::Point(4, 22);
            this->generalPage->Name = L"generalPage";
            this->generalPage->Padding = System::Windows::Forms::Padding(3);
            this->generalPage->Size = System::Drawing::Size(522, 440);
            this->generalPage->TabIndex = 0;
            this->generalPage->Text = L"General";
            this->generalPage->UseVisualStyleBackColor = true;
            // 
            // djGroup
            // 
            this->djGroup->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
                | System::Windows::Forms::AnchorStyles::Right));
            this->djGroup->Controls->Add(this->allowGuestLoginsCheckBox);
            this->djGroup->Location = System::Drawing::Point(6, 244);
            this->djGroup->Name = L"djGroup";
            this->djGroup->Size = System::Drawing::Size(510, 104);
            this->djGroup->TabIndex = 12;
            this->djGroup->TabStop = false;
            this->djGroup->Text = L"DJ";
            this->djGroup->Visible = false;
            // 
            // allowGuestLoginsCheckBox
            // 
            this->allowGuestLoginsCheckBox->AutoSize = true;
            this->allowGuestLoginsCheckBox->Location = System::Drawing::Point(6, 19);
            this->allowGuestLoginsCheckBox->Name = L"allowGuestLoginsCheckBox";
            this->allowGuestLoginsCheckBox->Size = System::Drawing::Size(229, 17);
            this->allowGuestLoginsCheckBox->TabIndex = 11;
            this->allowGuestLoginsCheckBox->Text = L"Allow guests to connect and request songs";
            this->allowGuestLoginsCheckBox->UseVisualStyleBackColor = true;
            this->allowGuestLoginsCheckBox->CheckedChanged += gcnew System::EventHandler(this, &PreferencesPageDialog::allowGuestLoginsCheckBox_CheckedChanged);
            // 
            // generalPageLogo
            // 
            this->generalPageLogo->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
            this->generalPageLogo->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"generalPageLogo.Image")));
            this->generalPageLogo->Location = System::Drawing::Point(291, 409);
            this->generalPageLogo->Name = L"generalPageLogo";
            this->generalPageLogo->Size = System::Drawing::Size(225, 25);
            this->generalPageLogo->SizeMode = System::Windows::Forms::PictureBoxSizeMode::AutoSize;
            this->generalPageLogo->TabIndex = 8;
            this->generalPageLogo->TabStop = false;
            // 
            // customizationPage
            // 
            this->customizationPage->Controls->Add(this->stopOnPauseCheckBox);
            this->customizationPage->Controls->Add(this->customizationPageLogo);
            this->customizationPage->Controls->Add(this->checkBox1);
            this->customizationPage->Controls->Add(formatStringsGroup);
            this->customizationPage->Location = System::Drawing::Point(4, 22);
            this->customizationPage->Name = L"customizationPage";
            this->customizationPage->Padding = System::Windows::Forms::Padding(3);
            this->customizationPage->Size = System::Drawing::Size(522, 440);
            this->customizationPage->TabIndex = 1;
            this->customizationPage->Text = L"Customization";
            this->customizationPage->UseVisualStyleBackColor = true;
            // 
            // stopOnPauseCheckBox
            // 
            this->stopOnPauseCheckBox->AutoSize = true;
            this->stopOnPauseCheckBox->Location = System::Drawing::Point(16, 242);
            this->stopOnPauseCheckBox->Name = L"stopOnPauseCheckBox";
            this->stopOnPauseCheckBox->Size = System::Drawing::Size(175, 17);
            this->stopOnPauseCheckBox->TabIndex = 10;
            this->stopOnPauseCheckBox->Text = L"Stop playback instead of pause";
            this->stopOnPauseCheckBox->UseVisualStyleBackColor = true;
            this->stopOnPauseCheckBox->CheckedChanged += gcnew System::EventHandler(this, &PreferencesPageDialog::stopOnPauseCheckBox_CheckedChanged);
            // 
            // customizationPageLogo
            // 
            this->customizationPageLogo->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
            this->customizationPageLogo->Location = System::Drawing::Point(291, 409);
            this->customizationPageLogo->Name = L"customizationPageLogo";
            this->customizationPageLogo->Size = System::Drawing::Size(225, 25);
            this->customizationPageLogo->SizeMode = System::Windows::Forms::PictureBoxSizeMode::AutoSize;
            this->customizationPageLogo->TabIndex = 9;
            this->customizationPageLogo->TabStop = false;
            // 
            // checkBox1
            // 
            this->checkBox1->AutoSize = true;
            this->checkBox1->Location = System::Drawing::Point(16, 219);
            this->checkBox1->Name = L"checkBox1";
            this->checkBox1->Size = System::Drawing::Size(210, 17);
            this->checkBox1->TabIndex = 1;
            this->checkBox1->Text = L"Sorts songs alphabetically in Tracks list";
            this->checkBox1->UseVisualStyleBackColor = true;
            this->checkBox1->Visible = false;
            // 
            // networkPage
            // 
            this->networkPage->Controls->Add(debugGroup);
            this->networkPage->Controls->Add(this->trafficCompressionCheckBox);
            this->networkPage->Controls->Add(this->portNumber);
            this->networkPage->Controls->Add(label2);
            this->networkPage->Controls->Add(this->networkPageLogo);
            this->networkPage->Location = System::Drawing::Point(4, 22);
            this->networkPage->Name = L"networkPage";
            this->networkPage->Padding = System::Windows::Forms::Padding(3);
            this->networkPage->Size = System::Drawing::Size(522, 440);
            this->networkPage->TabIndex = 2;
            this->networkPage->Text = L"Network";
            this->networkPage->UseVisualStyleBackColor = true;
            // 
            // trafficCompressionCheckBox
            // 
            this->trafficCompressionCheckBox->AutoSize = true;
            this->trafficCompressionCheckBox->Location = System::Drawing::Point(131, 14);
            this->trafficCompressionCheckBox->Name = L"trafficCompressionCheckBox";
            this->trafficCompressionCheckBox->Size = System::Drawing::Size(191, 17);
            this->trafficCompressionCheckBox->TabIndex = 15;
            this->trafficCompressionCheckBox->Text = L"Enable network traffic compression";
            this->trafficCompressionCheckBox->UseVisualStyleBackColor = true;
            this->trafficCompressionCheckBox->CheckedChanged += gcnew System::EventHandler(this, &PreferencesPageDialog::trafficCompressionCheckBox_CheckedChanged);
            // 
            // portNumber
            // 
            this->portNumber->Location = System::Drawing::Point(44, 13);
            this->portNumber->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {65535, 0, 0, 0});
            this->portNumber->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
            this->portNumber->Name = L"portNumber";
            this->portNumber->Size = System::Drawing::Size(66, 20);
            this->portNumber->TabIndex = 14;
            this->portNumber->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {7000, 0, 0, 0});
            this->portNumber->ValueChanged += gcnew System::EventHandler(this, &PreferencesPageDialog::portNumber_ValueChanged);
            // 
            // networkPageLogo
            // 
            this->networkPageLogo->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
            this->networkPageLogo->Location = System::Drawing::Point(291, 409);
            this->networkPageLogo->Name = L"networkPageLogo";
            this->networkPageLogo->Size = System::Drawing::Size(225, 25);
            this->networkPageLogo->SizeMode = System::Windows::Forms::PictureBoxSizeMode::AutoSize;
            this->networkPageLogo->TabIndex = 10;
            this->networkPageLogo->TabStop = false;
            // 
            // titleformattingError
            // 
            this->titleformattingError->ContainerControl = this;
            // 
            // PreferencesPageDialog
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->AutoValidate = System::Windows::Forms::AutoValidate::Disable;
            this->Controls->Add(this->tabPages);
            this->Name = L"PreferencesPageDialog";
            this->Size = System::Drawing::Size(530, 466);
            this->Load += gcnew System::EventHandler(this, &PreferencesPageDialog::PreferencesPageDialog_Load);
            devicesGroup->ResumeLayout(false);
            displayNameGroup->ResumeLayout(false);
            displayNameGroup->PerformLayout();
            formatStringsGroup->ResumeLayout(false);
            formatStringsGroup->PerformLayout();
            debugGroup->ResumeLayout(false);
            debugGroup->PerformLayout();
            this->tabPages->ResumeLayout(false);
            this->generalPage->ResumeLayout(false);
            this->generalPage->PerformLayout();
            this->djGroup->ResumeLayout(false);
            this->djGroup->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->generalPageLogo))->EndInit();
            this->customizationPage->ResumeLayout(false);
            this->customizationPage->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->customizationPageLogo))->EndInit();
            this->networkPage->ResumeLayout(false);
            this->networkPage->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->portNumber))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->networkPageLogo))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->titleformattingError))->EndInit();
            this->ResumeLayout(false);

        }
		#pragma endregion

	private: 
        System::Void ControlHandleCreated(System::Object^  sender, System::EventArgs^  e) 
		{
			Control^ ctl = (Control^)sender;
            if (ctl->IsHandleCreated)
            {
                HWND handle = (HWND)(void*)ctl->Handle;
                SetWindowLong(handle, GWL_EXSTYLE, GetWindowLong(handle, GWL_EXSTYLE) | WS_EX_CONTROLPARENT);
            }
		}

		System::Void serviceName_TextChanged(System::Object^  sender, System::EventArgs^  e) 
		{
			State |= preferences_state::changed | preferences_state::needs_restart;
		}

		System::Void portNumber_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
		{
			State |= preferences_state::changed | preferences_state::needs_restart;
		}

		System::Void displayName_TextChanged(System::Object^  sender, System::EventArgs^  e) 
		{
			State |= preferences_state::changed | preferences_state::needs_restart;
		}
	private: 
		void LoadDeviceList()
		{
			devicesList->BeginUpdate();
			devicesList->Items->Clear();
			for each (PairedDevice^ dev in TouchRemote::Core::Pairing::PairedDevices::List)
				devicesList->Items->Add(dev);
			devicesList->EndUpdate();
		}

		void UpdateDeviceState()
		{
			deleteSelectedDeviceButton->Enabled = devicesList->SelectedItem != nullptr;
			deleteAllDevicesButton->Enabled = devicesList->Items->Count > 0;
		}

		System::Void devicesList_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) 
		{
			UpdateDeviceState();
		}

		void ForgetDevice(Object^ obj)
		{
			PairedDevice^ dev = safe_cast<PairedDevice^>(obj);
			if (dev == nullptr) return;

			try
			{
				PairedDevices::ForgetDevice(dev);
				devicesList->Items->Remove(dev);
			}
			catch (Exception^ ex)
			{
				_console::error(ex->ToString());
			}
		}

		System::Void deleteSelectedDeviceButton_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			ForgetDevice(devicesList->SelectedItem);
			UpdateDeviceState();
		}

		System::Void deleteAllDevicesButton_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			array<Object^>^ items = gcnew array<Object^>(devicesList->Items->Count);
			devicesList->Items->CopyTo(items, 0);

			for each (Object^ obj in items)
				ForgetDevice(obj);

			UpdateDeviceState();				
		}

		System::Void addNewDeviceButton_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			String^ strDatabaseId = ManagedHost::Instance->DatabaseId.ToString("X16");

			PairingDialog^ dlg = gcnew PairingDialog(strDatabaseId);
							
			dlg->ShowDialog();

			LoadDeviceList();
			UpdateDeviceState();
		}
	
		System::Void httpLoggingCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
		{
			State |= preferences_state::changed;
		}
	
		System::Void TitleFormat_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) 
		{
			TextBox^ txt = safe_cast<TextBox^>(sender);
			if (txt == nullptr) return;

			if (!String::IsNullOrEmpty(txt->Text))
			{
				static_api_ptr_t<titleformat_compiler> compiler;
				titleformat_object::ptr obj;

				if (!compiler->compile(obj, ToUtf8String(txt->Text)))
				{
					titleformattingError->SetError(txt, "Invalid pattern");
					e->Cancel = true;
					return;
				}
			}

			titleformattingError->SetError(txt, nullptr);
			e->Cancel = false;
		}
	
		System::Void TitleFormat_Validated(System::Object^  sender, System::EventArgs^  e) 
		{
			//State |= preferences_state::changed | preferences_state::needs_restart;
		}
	
		System::Void TitleFormat_TextChanged(System::Object^  sender, System::EventArgs^  e) 
		{
			State |= preferences_state::changed | preferences_state::needs_restart;
		}

		System::Void trafficCompressionCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
		{
			State |= preferences_state::changed;
		}

		System::Void allowGuestLoginsCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
		{
			State |= preferences_state::changed | preferences_state::needs_restart;
		}

        System::Void stopOnPauseCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
        {
            State |= preferences_state::changed;
        }
    
        System::Void PreferencesPageDialog_Load(System::Object^  sender, System::EventArgs^  e) 
        {
            generalPage->CreateControl();
        }
    };

}
