#include "InspectorWidget.h"
#include <Runtime.h>

namespace editor
{
	InspectorWidget::InspectorWidget(QWidget* parent /*= 0*/)
	{
		ui.setupUi(this);

		m_BoolManager = new QtBoolPropertyManager(this);
		m_IntManager = new QtIntPropertyManager(this);
		m_DoubleManager = new QtDoublePropertyManager(this);
		m_StringManager = new QtStringPropertyManager(this);
		m_Vec3Manager = new QtVec3PropertyManager(this);
		m_ColorManager = new QtColorPropertyManager(this);
		m_EnumManager = new QtEnumPropertyManager(this);
		m_GroupManager = new QtGroupPropertyManager(this);

		connect(m_BoolManager, SIGNAL(valueChanged(QtProperty *, bool)), this, SLOT(ValueChanged(QtProperty *, bool)));
		connect(m_IntManager, SIGNAL(valueChanged(QtProperty *, int)), this, SLOT(ValueChanged(QtProperty *, int)));
		connect(m_DoubleManager, SIGNAL(valueChanged(QtProperty *, double)), this, SLOT(ValueChanged(QtProperty *, double)));
		connect(m_StringManager, SIGNAL(valueChanged(QtProperty *, const QString &)), this, SLOT(ValueChanged(QtProperty *, const QString &)));
		connect(m_Vec3Manager, SIGNAL(valueChanged(QtProperty *, const glm::vec3 &)), this, SLOT(ValueChanged(QtProperty *, const glm::vec3 &)));
		connect(m_ColorManager, SIGNAL(valueChanged(QtProperty *, const QColor &)), this, SLOT(ValueChanged(QtProperty *, const QColor &)));

		QtCheckBoxFactory* checkBoxFactory = new QtCheckBoxFactory(this);
		QtSpinBoxFactory* spinBoxFactory = new QtSpinBoxFactory(this);
		QtDoubleSpinBoxFactory* doubleSpinBoxFactory = new QtDoubleSpinBoxFactory(this);
		QtSliderFactory* m_SliderFactory = new QtSliderFactory(this);
		QtScrollBarFactory* scrollBarFactory = new QtScrollBarFactory(this);
		QtLineEditFactory* lineEditFactory = new QtLineEditFactory(this);
		QtColorEditorFactory* colorEditFactory = new QtColorEditorFactory(this);
		QtVec3EditorFactory* vec3EditFactory = new QtVec3EditorFactory(this);
		QtEnumEditorFactory* comboBoxFactorynew = new QtEnumEditorFactory(this);

		m_PropertyBrowser = new QtButtonPropertyBrowser();
		m_PropertyBrowser->setFactoryForManager(m_BoolManager, checkBoxFactory);
		m_PropertyBrowser->setFactoryForManager(m_IntManager, spinBoxFactory);
		m_PropertyBrowser->setFactoryForManager(m_DoubleManager, doubleSpinBoxFactory);
		m_PropertyBrowser->setFactoryForManager(m_StringManager, lineEditFactory);
		m_PropertyBrowser->setFactoryForManager(m_Vec3Manager, vec3EditFactory);
		m_PropertyBrowser->setFactoryForManager(m_ColorManager, colorEditFactory);
		m_PropertyBrowser->setFactoryForManager(m_EnumManager, comboBoxFactorynew);

		ui.scrollArea->setWidgetResizable(true);
		ui.scrollArea->setWidget(m_PropertyBrowser);
	}

	void InspectorWidget::SelectionChanged(core::Object* selection)
	{
		m_Selection = selection;

		if (selection == nullptr)
		{
			m_PropertyBrowser->clear();

			return;
		}

		auto& type = m_Selection->GetType();

		//support only for game::GameObject for now
		auto& game_object_type = typeof(game::GameObject);
		if (type == game_object_type)
		{
			m_PropertyBrowser->clear();

			game::GameObject* game_object = static_cast<game::GameObject*>(m_Selection);

			QtProperty* name_property = m_StringManager->addProperty("Name");
			name_property->setType(InspectorItemType::GAME_OBJECT_NAME);
			QVariant data;
			data.setValue<game::GameObject*>(game_object);
			name_property->setData(0, data);
			auto name = game_object->GetName();

			m_StringManager->setValue(name_property, QString::fromStdString(name));

			QtBrowserItem* name_item = m_PropertyBrowser->addProperty(name_property);

			auto& components_any = game_object->GetComponentsAny();
			for (auto& pair : components_any)
			{
				auto& id = pair.first;
				auto& component_type = reflection::Type(id);
				auto& component_any = pair.second;

				auto display_name_attribute = component_type.GetAttribute<attribute::DisplayName>();
				std::string component_name = display_name_attribute->Value;

				QVariant component_data;
				component_data.setValue<reflection::Any>(component_any);

				QtProperty* component_property = m_GroupManager->addProperty(QString::fromStdString(component_name));
				auto& fields = component_type.GetFields();
				for (auto i = fields.begin(); i != fields.end(); ++i)
				{
					auto& field = (*i);
					auto field_display_name_attribute = field.GetAttribute<attribute::DisplayName>();
					std::string field_name = field_display_name_attribute->Value;
					auto& value_any = field.GetValue(component_any);

					QtProperty* field_property = nullptr;
					if (field.GetType() == typeof(int))
					{
						field_property = m_IntManager->addProperty(QString::fromStdString(field_name));
						m_IntManager->setValue(field_property, value_any.GetValue<int>());
					}
					else if (field.GetType() == typeof(float))
					{
						field_property = m_DoubleManager->addProperty(QString::fromStdString(field_name));
						m_DoubleManager->setValue(field_property, (double)(value_any.GetValue<float>()));
					}
					else if (field.GetType() == typeof(std::string))
					{
						field_property = m_StringManager->addProperty(QString::fromStdString(field_name));
						m_StringManager->setValue(field_property, QString::fromStdString(value_any.GetValue<std::string>()));
					}
					else if (field.GetType() == typeof(glm::vec3))
					{
						field_property = m_Vec3Manager->addProperty(QString::fromStdString(field_name));
						m_Vec3Manager->setValue(field_property, value_any.GetValue<glm::vec3>());
					}
					else
					{
						//TODO!!!
					}

					if (field_property != nullptr)
					{
						field_property->setType(InspectorItemType::FIELD);
						QVariant field_data;
						field_data.setValue<reflection::Field>(field);
						
						field_property->setData(0, component_data);
						field_property->setData(1, field_data);

						component_property->addSubProperty(field_property);
					}
				}

				QtBrowserItem* component_item = m_PropertyBrowser->addProperty(component_property);
				m_PropertyBrowser->setExpanded(component_item, true);
			}
		}
	}

	void InspectorWidget::ValueChanged(QtProperty *property, bool value)
	{
		int x = 3;
		//TODO!!!
	}
	void InspectorWidget::ValueChanged(QtProperty *property, int value)
	{
		if (property == nullptr)
			return;

		//m_IntManager->setValue(property, value);

		if (property->type() == InspectorItemType::FIELD)
		{
			QVariant component_data = property->data(0);
			QVariant field_data = property->data(1);
			auto& field = field_data.value<reflection::Field>();
			auto& component_any = component_data.value<reflection::Any>();

			field.SetValue(component_any, reflection::Any(value));
		}

		int x = 3;
		//TODO!!!
	}
	void InspectorWidget::ValueChanged(QtProperty *property, double value)
	{
		if (property == nullptr)
			return;

		//m_DoubleManager->setValue(property, value);

		if (property->type() == InspectorItemType::FIELD)
		{
			QVariant component_data = property->data(0);
			QVariant field_data = property->data(1);
			auto& field = field_data.value<reflection::Field>();
			auto& component_any = component_data.value<reflection::Any>();

			field.SetValue(component_any, reflection::Any((float)value));
		}

		int x = 3;
		//TODO!!!
	}
	void InspectorWidget::ValueChanged(QtProperty *property, const QString &value)
	{
		if (property == nullptr)
			return;

		//m_StringManager->setValue(property, value);

		if (property->type() == InspectorItemType::GAME_OBJECT_NAME)
		{
			QVariant data = property->data(0);
			game::GameObject* game_object = data.value<game::GameObject*>();
			if (game_object != nullptr)
			{
				game_object->SetName(value.toStdString());
			}
		}
		if (property->type() == InspectorItemType::FIELD)
		{
			QVariant component_data = property->data(0);
			QVariant field_data = property->data(1);
			auto& field = field_data.value<reflection::Field>();
			auto& component_any = component_data.value<reflection::Any>();

			field.SetValue(component_any, reflection::Any(value.toStdString()));
		}

		int x = 3;
		//TODO!!!
	}

	void InspectorWidget::ValueChanged(QtProperty* property, const glm::vec3 &value)
	{
		if (property == nullptr)
			return;

		//m_Vec3Manager->setValue(property, value);

		if (property->type() == InspectorItemType::FIELD)
		{
			QVariant component_data = property->data(0);
			QVariant field_data = property->data(1);
			auto& field = field_data.value<reflection::Field>();
			auto& component_any = component_data.value<reflection::Any>();

			field.SetValue(component_any, reflection::Any(value));
		}

		int x = 3;
		//TODO!!!
	}

	void InspectorWidget::ValueChanged(QtProperty *property, const QColor &value)
	{
		int x = 3;
		//TODO!!!
	}
}