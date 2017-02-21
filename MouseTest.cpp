
#include <Core/CoreAll.h>
#include <Fusion/FusionAll.h>
#include <CAM/CAMAll.h>


using namespace adsk::core;
using namespace adsk::fusion;
using namespace adsk::cam;

Ptr<Application> app;
Ptr<UserInterface> ui;

const std::string commandId = "MouseTestCPP";
const std::string panelId = "SolidScriptsAddinsPanel";


// CommandExecuted event handler.
class OnExecuteEventHander : public CommandEventHandler
{
public:
	void notify(const Ptr<CommandEventArgs>& eventArgs) override
	{
		if (!app)
			return;

		 //do nothing, just pop out the dialog
	}
};

class MyMouseClickHandler : public MouseEventHandler
{
public:
	void notify(const Ptr<MouseEventArgs>& eventArgs) override
	{


		Ptr<Event> firingEvent = eventArgs->firingEvent();
		if (!firingEvent)
			return;

		Ptr<Command> command = firingEvent->sender();
		if (!command)
			return;

		Ptr<CommandInputs> inputs = command->commandInputs();
		if (!inputs)
			return;

		Ptr<TextBoxCommandInput>  txtBox = inputs->itemById("clickResults");
		 

		std::string str = "{";
		str += std::to_string(eventArgs->position()->x());
		str += ",";
		str += std::to_string(eventArgs->position()->y());
		str += "}";
		 
		txtBox->text(str);

	}
};


class MyMouseMoveHandler : public MouseEventHandler
{
public:
	void notify(const Ptr<MouseEventArgs>& eventArgs) override
	{


		Ptr<Event> firingEvent = eventArgs->firingEvent();
		if (!firingEvent)
			return;

		Ptr<Command> command = firingEvent->sender();
		if (!command)
			return;

		Ptr<CommandInputs> inputs = command->commandInputs();
		if (!inputs)
			return;

		Ptr<TextBoxCommandInput>  txtBox = inputs->itemById("moveResults");

		std::string str = "{";
		str += std::to_string(eventArgs->position()->x());
		str += ",";
		str += std::to_string(eventArgs->position()->y());
		str += "}";


		txtBox->text(str);

		Ptr<Point2D> mousePoint = eventArgs->position();
			if (!mousePoint)
				return; 

			Ptr<Viewport> activeViewPoint = app->activeViewport();
			if (!activeViewPoint)
				return;

			Ptr<Point3D> modelPoint = activeViewPoint->viewToModelSpace(mousePoint);
			if (!modelPoint)
				return; 

			double mx = modelPoint->x();
			double my = modelPoint->y();
			double mz = modelPoint->z();

			Ptr<Camera> camera = activeViewPoint->camera();
			if (!camera)
				return;		

			Ptr<Point3D> eyePoint = camera->eye();
			if (!eyePoint)
				return;

			double ecx = eyePoint->x();
			double ey = eyePoint->y();
			double ez = eyePoint->z();

			Ptr<Vector3D> rayVector = eyePoint->vectorTo(modelPoint);
			if (!rayVector)
				return;

			Ptr<Design> design = app->activeProduct();
			if (!design)
				return;
	
			Ptr<Component> component = design->rootComponent();
			if (!component)
				return;

			Ptr<ObjectCollection> hitEntities = component->findBRepUsingRay(eyePoint, rayVector, BRepFaceEntityType);
			if (!hitEntities)
				return;

			Ptr<CommandInputs> commandInputs = command->commandInputs();
			if (!commandInputs)
				return;

			Ptr<SelectionCommandInput> selectInput = commandInputs->itemById("myselection");
			if (!selectInput)
				return;

			selectInput->clearSelection();
			for (Ptr<BRepFace> face : hitEntities)
			{
				selectInput->addSelection(face);
			} 


	}
};

 
class MyMouseWheelHandler : public MouseEventHandler
{
public:
	void notify(const Ptr<MouseEventArgs>& eventArgs) override
	{
		 
	
		Ptr<Event> firingEvent = eventArgs->firingEvent();
		if (!firingEvent)
			return;

		Ptr<Command> command = firingEvent->sender();
		if (!command)
			return;

		Ptr<CommandInputs> inputs = command->commandInputs();
		if (!inputs)
			return;

		Ptr<TextBoxCommandInput>  txtBox = inputs->itemById("wheelResults");
 

		txtBox->text(std::to_string(eventArgs->wheelDelta()));
		 
		 
	}
};


// CommandCreated event handler.
class OnCommandCreatedEventHandler : public CommandCreatedEventHandler
{
public:
	void notify(const Ptr<CommandCreatedEventArgs>& eventArgs) override
	{
		if (eventArgs)
		{
			Ptr<Command> cmd = eventArgs->command();
			if (cmd)
			{
				// Connect to the CommandExecuted event.
				Ptr<CommandEvent> onExec = cmd->execute();
				if (!onExec)
					return;
				bool isOk = onExec->add(&onExecuteHander_);
				if (!isOk)
					return;

				 

				// Define the inputs.
				Ptr<CommandInputs> inputs = cmd->commandInputs();
				if (!inputs)
					return;
				
				Ptr<SelectionCommandInput> 
					selectionCommandInput = inputs->addSelectionInput("myselection", "Selection", "");

				//add inputs 
				inputs->addTextBoxCommandInput("clickResults", "Click", "",1,true);
				inputs->addTextBoxCommandInput("doubleClickResults", "Double Click", "", 1, true);
				inputs->addTextBoxCommandInput("downResults", "Down", "", 1, true);
				inputs->addTextBoxCommandInput("dragResults", "Drag", "", 1, true);
				inputs->addTextBoxCommandInput("dragBeginResults", "Drag Begin", "", 1, true);
				inputs->addTextBoxCommandInput("dragEndResults", "Drag end", "", 1, true);
				inputs->addTextBoxCommandInput("moveResults", "Move", "", 1, true);
				inputs->addTextBoxCommandInput("upResults", "Up", "", 1, true);
				inputs->addTextBoxCommandInput("wheelResults", "Wheel", "", 1, true);

				//add mouse events

				// Connect to the MouseEvent.
				cmd->mouseClick()->add(&onMouseClickHandler_); 
				cmd->mouseMove()->add(&onMouseMoveHandler_);
				cmd->mouseWheel()->add(&onMouseWheelHandler_);
				 

			}
		}
	}
private:
	OnExecuteEventHander onExecuteHander_;
	MyMouseClickHandler onMouseClickHandler_;
	MyMouseMoveHandler onMouseMoveHandler_;
	MyMouseWheelHandler onMouseWheelHandler_;


 } cmdCreated_;


extern "C" XI_EXPORT bool run(const char* context)
{
	const std::string commandName = "Mouse Test in C++";
	const std::string commandDescription = "This is to test mouse event in C++";
	const std::string commandResources = "./resources";

	app = Application::get();
	if (!app)
		return false;

	ui = app->userInterface();
	if (!ui)
		return false;

	// add a command on create panel in modeling workspace
	Ptr<Workspaces> workspaces = ui->workspaces();
	if (!workspaces)
		return false;
	Ptr<Workspace> modelingWorkspace = workspaces->itemById("FusionSolidEnvironment");
	if (!modelingWorkspace)
		return false;

	Ptr<ToolbarPanels> toolbarPanels = modelingWorkspace->toolbarPanels();
	if (!toolbarPanels)
		return false;
	Ptr<ToolbarPanel> toolbarPanel = toolbarPanels->itemById(panelId);
	if (!toolbarPanel)
		return false;

	Ptr<ToolbarControls> toolbarControls = toolbarPanel->controls();
	if (!toolbarControls)
		return false;
	Ptr<ToolbarControl> toolbarControl = toolbarControls->itemById(commandId);
	if (toolbarControl)
	{
 		ui->messageBox("[Mouse Test in C++] command is already loaded.");
		adsk::terminate();
		return true;
	}
	else
	{
		Ptr<CommandDefinitions> commandDefinitions = ui->commandDefinitions();
		if (!commandDefinitions)
			return false;
		Ptr<CommandDefinition> commandDefinition = commandDefinitions->itemById(commandId);
		if (!commandDefinition)
		{
			commandDefinition = commandDefinitions->addButtonDefinition(commandId, commandName, commandDescription, commandResources);
			if (!commandDefinition)
				return false;
		}

		Ptr<CommandCreatedEvent> commandCreatedEvent = commandDefinition->commandCreated();
		if (!commandCreatedEvent)
			return false;
		commandCreatedEvent->add(&cmdCreated_);
		toolbarControl = toolbarControls->addCommand(commandDefinition);
		if (!toolbarControl)
			return false;
		toolbarControl->isVisible(true);
	}




	//ui->messageBox("in run");

	return true;
}

extern "C" XI_EXPORT bool stop(const char* context)
{
	

	if (!app)
		return false;

	if (!ui)
		return false;

	// Get toolbar control to delete
	Ptr<Workspaces> workspaces = ui->workspaces();
	if (!workspaces)
		return false;
	Ptr<Workspace> modelingWorkspace = workspaces->itemById("FusionSolidEnvironment");
	if (!modelingWorkspace)
		return false;

	Ptr<ToolbarPanels> toolbarPanels = modelingWorkspace->toolbarPanels();
	if (!toolbarPanels)
		return false;
	Ptr<ToolbarPanel> toolbarPanel = toolbarPanels->itemById(panelId);
	if (!toolbarPanel)
		return false;

	Ptr<ToolbarControls> toolbarControls = toolbarPanel->controls();
	if (!toolbarControls)
		return false;
	Ptr<ToolbarControl> toolbarControl = toolbarControls->itemById(commandId);

	// Get command definition to delete
	Ptr<CommandDefinitions> commandDefinitions = ui->commandDefinitions();
	if (!commandDefinitions)
		return false;
	Ptr<CommandDefinition> commandDefinition = commandDefinitions->itemById(commandId);

	if (toolbarControl)
		toolbarControl->deleteMe();

	if (commandDefinition)
		commandDefinition->deleteMe();

	if (ui)
	{
		//ui->messageBox("in stop");
		ui = nullptr;
	}

	return true;
}




#ifdef XI_WIN

#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hmodule, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#endif // XI_WIN
