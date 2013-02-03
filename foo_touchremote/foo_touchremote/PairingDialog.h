#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace TouchRemote::Bonjour;
using namespace TouchRemote::Core::Pairing;

#pragma managed

#include "Utils.h"

namespace foo_touchremote 
{
	/// <summary>
	/// Summary for PairingDialog
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class PairingDialog : public System::Windows::Forms::Form
	{
	private:
		String^ m_serviceName;
	private: System::ComponentModel::BackgroundWorker^  backgroundWorker;
			 BonjourResolver^ m_resolver;
	private: System::Windows::Forms::Panel^  searchPanel;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::PictureBox^  pictureBox1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Panel^  pairingPanel;

	private: System::Windows::Forms::ProgressBar^  progressBar1;
	private: System::Windows::Forms::Button^  cancelButton1;
	private: System::Windows::Forms::Button^  confirmPairingButton;

	private: System::Windows::Forms::Button^  cancelButton2;
	private: System::Windows::Forms::PictureBox^  pictureBox2;
	private: System::Windows::Forms::MaskedTextBox^  pinCodeTextBox;

	private: System::Windows::Forms::Label^  label3;



			 IClientDevice^ m_device;

	public:
		PairingDialog(String^ serviceName)
		{
			if (String::IsNullOrEmpty(serviceName))
				throw gcnew ArgumentNullException("serviceName");

			m_serviceName = serviceName;
			m_resolver = gcnew BonjourResolver();
			m_device = nullptr;

			InitializeComponent();

			searchPanel->Visible = true;
			pairingPanel->Visible = false;

			pairingPanel->Location = searchPanel->Location;
			pairingPanel->Size = searchPanel->Size;

			CancelButton = cancelButton1;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~PairingDialog()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(PairingDialog::typeid));
			this->backgroundWorker = (gcnew System::ComponentModel::BackgroundWorker());
			this->searchPanel = (gcnew System::Windows::Forms::Panel());
			this->cancelButton1 = (gcnew System::Windows::Forms::Button());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->pairingPanel = (gcnew System::Windows::Forms::Panel());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->pinCodeTextBox = (gcnew System::Windows::Forms::MaskedTextBox());
			this->pictureBox2 = (gcnew System::Windows::Forms::PictureBox());
			this->cancelButton2 = (gcnew System::Windows::Forms::Button());
			this->confirmPairingButton = (gcnew System::Windows::Forms::Button());
			this->searchPanel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
			this->pairingPanel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox2))->BeginInit();
			this->SuspendLayout();
			// 
			// backgroundWorker
			// 
			this->backgroundWorker->WorkerSupportsCancellation = true;
			this->backgroundWorker->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &PairingDialog::backgroundWorker_DoWork);
			this->backgroundWorker->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &PairingDialog::backgroundWorker_RunWorkerCompleted);
			// 
			// searchPanel
			// 
			this->searchPanel->Controls->Add(this->cancelButton1);
			this->searchPanel->Controls->Add(this->progressBar1);
			this->searchPanel->Controls->Add(this->label2);
			this->searchPanel->Controls->Add(this->pictureBox1);
			this->searchPanel->Controls->Add(this->label1);
			this->searchPanel->Location = System::Drawing::Point(3, 6);
			this->searchPanel->Name = L"searchPanel";
			this->searchPanel->Size = System::Drawing::Size(303, 149);
			this->searchPanel->TabIndex = 0;
			// 
			// cancelButton1
			// 
			this->cancelButton1->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->cancelButton1->Location = System::Drawing::Point(114, 117);
			this->cancelButton1->Name = L"cancelButton1";
			this->cancelButton1->Size = System::Drawing::Size(75, 23);
			this->cancelButton1->TabIndex = 0;
			this->cancelButton1->Text = L"Cancel";
			this->cancelButton1->UseVisualStyleBackColor = true;
			this->cancelButton1->Click += gcnew System::EventHandler(this, &PairingDialog::cancelButton1_Click);
			// 
			// progressBar1
			// 
			this->progressBar1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->progressBar1->Location = System::Drawing::Point(78, 37);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(213, 23);
			this->progressBar1->Style = System::Windows::Forms::ProgressBarStyle::Marquee;
			this->progressBar1->TabIndex = 3;
			// 
			// label2
			// 
			this->label2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->label2->Location = System::Drawing::Point(3, 70);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(297, 44);
			this->label2->TabIndex = 2;
			this->label2->Text = L"Now launch Remote application on your device and choose \"Add an iTunes Library\" o" 
				L"ption";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->label2->UseMnemonic = false;
			// 
			// pictureBox1
			// 
			this->pictureBox1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pictureBox1.Image")));
			this->pictureBox1->Location = System::Drawing::Point(3, 3);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(64, 64);
			this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::AutoSize;
			this->pictureBox1->TabIndex = 1;
			this->pictureBox1->TabStop = false;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Tahoma", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(204)));
			this->label1->Location = System::Drawing::Point(73, 3);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(222, 27);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Waiting for Remote...";
			// 
			// pairingPanel
			// 
			this->pairingPanel->Controls->Add(this->label3);
			this->pairingPanel->Controls->Add(this->pinCodeTextBox);
			this->pairingPanel->Controls->Add(this->pictureBox2);
			this->pairingPanel->Controls->Add(this->cancelButton2);
			this->pairingPanel->Controls->Add(this->confirmPairingButton);
			this->pairingPanel->Location = System::Drawing::Point(3, 174);
			this->pairingPanel->Name = L"pairingPanel";
			this->pairingPanel->Size = System::Drawing::Size(303, 149);
			this->pairingPanel->TabIndex = 1;
			// 
			// label3
			// 
			this->label3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->label3->Location = System::Drawing::Point(3, 70);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(297, 44);
			this->label3->TabIndex = 9;
			this->label3->Text = L"To complete pairing enter a four-digit security code displayed on your device scr" 
				L"een";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// pinCodeTextBox
			// 
			this->pinCodeTextBox->AllowPromptAsInput = false;
			this->pinCodeTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->pinCodeTextBox->BeepOnError = true;
			this->pinCodeTextBox->CutCopyMaskFormat = System::Windows::Forms::MaskFormat::ExcludePromptAndLiterals;
			this->pinCodeTextBox->Font = (gcnew System::Drawing::Font(L"Tahoma", 36, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(238)));
			this->pinCodeTextBox->Location = System::Drawing::Point(78, 3);
			this->pinCodeTextBox->Mask = L"0 0 0 0";
			this->pinCodeTextBox->Name = L"pinCodeTextBox";
			this->pinCodeTextBox->Size = System::Drawing::Size(213, 65);
			this->pinCodeTextBox->TabIndex = 0;
			this->pinCodeTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->pinCodeTextBox->TextMaskFormat = System::Windows::Forms::MaskFormat::ExcludePromptAndLiterals;
			this->pinCodeTextBox->TextChanged += gcnew System::EventHandler(this, &PairingDialog::pinCodeTextBox_TextChanged);
			// 
			// pictureBox2
			// 
			this->pictureBox2->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pictureBox2.Image")));
			this->pictureBox2->Location = System::Drawing::Point(3, 3);
			this->pictureBox2->Name = L"pictureBox2";
			this->pictureBox2->Size = System::Drawing::Size(64, 64);
			this->pictureBox2->SizeMode = System::Windows::Forms::PictureBoxSizeMode::AutoSize;
			this->pictureBox2->TabIndex = 7;
			this->pictureBox2->TabStop = false;
			// 
			// cancelButton2
			// 
			this->cancelButton2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->cancelButton2->Location = System::Drawing::Point(156, 117);
			this->cancelButton2->Name = L"cancelButton2";
			this->cancelButton2->Size = System::Drawing::Size(75, 23);
			this->cancelButton2->TabIndex = 2;
			this->cancelButton2->Text = L"Cancel";
			this->cancelButton2->UseVisualStyleBackColor = true;
			this->cancelButton2->Click += gcnew System::EventHandler(this, &PairingDialog::cancelButton1_Click);
			// 
			// confirmPairingButton
			// 
			this->confirmPairingButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->confirmPairingButton->Enabled = false;
			this->confirmPairingButton->Location = System::Drawing::Point(62, 117);
			this->confirmPairingButton->Name = L"confirmPairingButton";
			this->confirmPairingButton->Size = System::Drawing::Size(75, 23);
			this->confirmPairingButton->TabIndex = 1;
			this->confirmPairingButton->Text = L"Confirm";
			this->confirmPairingButton->UseVisualStyleBackColor = true;
			this->confirmPairingButton->Click += gcnew System::EventHandler(this, &PairingDialog::confirmPairingButton_Click);
			// 
			// PairingDialog
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(310, 160);
			this->Controls->Add(this->pairingPanel);
			this->Controls->Add(this->searchPanel);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
			this->Name = L"PairingDialog";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"Pairing with a new device";
			this->Load += gcnew System::EventHandler(this, &PairingDialog::PairingDialog_Load);
			this->PreviewKeyDown += gcnew System::Windows::Forms::PreviewKeyDownEventHandler(this, &PairingDialog::PairingDialog_PreviewKeyDown);
			this->searchPanel->ResumeLayout(false);
			this->searchPanel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
			this->pairingPanel->ResumeLayout(false);
			this->pairingPanel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox2))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: 
		System::Void PairingDialog_Load(System::Object^  sender, System::EventArgs^  e) 
		{
			backgroundWorker->RunWorkerAsync();
			_console::print("TouchRemote: Looking for device...");
		}

		System::Void backgroundWorker_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e) 
		{
			e->Result = m_resolver->FindDevices("_touch-remote._tcp", "local.");
		}

		System::Void backgroundWorker_RunWorkerCompleted(System::Object^  sender, System::ComponentModel::RunWorkerCompletedEventArgs^  e) 
		{
			if (e->Cancelled) return;

			if (!IsHandleCreated || IsDisposed) return;

			System::Collections::Generic::IList<IClientDevice^>^ devs = safe_cast<System::Collections::Generic::IList<IClientDevice^>^>(e->Result);
			if (devs == nullptr || devs->Count == 0)
			{
				_console::print("TouchRemote: No devices found");
				MessageBox::Show("No devices were found :(");
				Close();
				return;
			}

			m_device = devs[0];

			_console::printf("TouchRemote: Found device \"{0}\" ({1}) @ {2}", m_device->Name, m_device->DeviceId, m_device->Host);

			searchPanel->Visible = false;
			pairingPanel->Visible = true;

			CancelButton = cancelButton2;
			AcceptButton = confirmPairingButton;

			pinCodeTextBox->Text = "";
			pinCodeTextBox->Focus();
		}

		System::Void cancelButton1_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			static_cast<Button^>(sender)->Enabled = false;
			backgroundWorker->CancelAsync();
			m_resolver->CancelFind();

			if (m_device == nullptr)
			{
				_console::print("TouchRemote: Search cancelled");
			}
			else
			{
				_console::print("TouchRemote: Pairing cancelled");
			}

			m_device = nullptr;
			Close();
		}

		System::Void pinCodeTextBox_MaskInputRejected(System::Object^  sender, System::Windows::Forms::MaskInputRejectedEventArgs^  e) 
		{
			confirmPairingButton->Enabled = false;
		}

		System::Void confirmPairingButton_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			if (m_device == nullptr) return;
			if (!pinCodeTextBox->MaskFull) return;
			
			String^ pinCode = pinCodeTextBox->Text;

			_console::printf("TouchRemote: trying to pair with  \"{0}\" ({1})", m_device->Name, m_device->DeviceId);
			try
			{
				PairedDevices::PerformPairing(m_device, pinCode, m_serviceName);
				_console::printf("TouchRemote: Successfully paired");
				Close();
			}
			catch (Exception^ ex)
			{
				_console::printf("TouchRemote: Pairing failed: {0}", ex->Message);
				MessageBox::Show(ex->Message);
			}
		}

		System::Void pinCodeTextBox_TextChanged(System::Object^  sender, System::EventArgs^  e) 
		{
			confirmPairingButton->Enabled = (m_device != nullptr) && pinCodeTextBox->MaskFull;
		}
	
		System::Void PairingDialog_PreviewKeyDown(System::Object^  sender, System::Windows::Forms::PreviewKeyDownEventArgs^  e) 
		{
			if (e->KeyCode == Keys::Escape)
			{
				Button^ b = (pairingPanel->Visible) ? cancelButton2 : cancelButton1;

				cancelButton1_Click(b, EventArgs::Empty);
			}
			else if (e->KeyCode == Keys::Enter && confirmPairingButton->Enabled)
			{
				confirmPairingButton_Click(confirmPairingButton, EventArgs::Empty);
			}
			 
		}
};
}
