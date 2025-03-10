// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#include "vtkInteractorStyleSwitch.h"

#include "vtkCallbackCommand.h"
#include "vtkCommand.h"
#include "vtkInteractorStyleJoystickActor.h"
#include "vtkInteractorStyleJoystickCamera.h"
#include "vtkInteractorStyleMultiTouchCamera.h"
#include "vtkInteractorStyleTrackballActor.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindowInteractor.h"

VTK_ABI_NAMESPACE_BEGIN
vtkStandardNewMacro(vtkInteractorStyleSwitch);

//------------------------------------------------------------------------------
vtkInteractorStyleSwitch::vtkInteractorStyleSwitch()
{
  this->JoystickActor = vtkInteractorStyleJoystickActor::New();
  this->JoystickCamera = vtkInteractorStyleJoystickCamera::New();
  this->TrackballActor = vtkInteractorStyleTrackballActor::New();
  this->TrackballCamera = vtkInteractorStyleTrackballCamera::New();
  this->MultiTouchCamera = vtkInteractorStyleMultiTouchCamera::New();
  this->JoystickOrTrackball = VTKIS_JOYSTICK;
  this->CameraOrActor = VTKIS_CAMERA;
  this->MultiTouch = false;
  this->CurrentStyle = nullptr;
}

//------------------------------------------------------------------------------
vtkInteractorStyleSwitch::~vtkInteractorStyleSwitch()
{
  this->JoystickActor->Delete();
  this->JoystickActor = nullptr;

  this->JoystickCamera->Delete();
  this->JoystickCamera = nullptr;

  this->TrackballActor->Delete();
  this->TrackballActor = nullptr;

  this->TrackballCamera->Delete();
  this->TrackballCamera = nullptr;

  this->MultiTouchCamera->Delete();
  this->MultiTouchCamera = nullptr;
}

//------------------------------------------------------------------------------
void vtkInteractorStyleSwitch::SetAutoAdjustCameraClippingRange(vtkTypeBool value)
{
  if (value == this->AutoAdjustCameraClippingRange)
  {
    return;
  }

#if !VTK_USE_FUTURE_BOOL
  if (value < 0 || value > 1)
  {
    vtkErrorMacro("Value must be 0 or 1 for"
      << " SetAutoAdjustCameraClippingRange");
    return;
  }
#endif

  this->AutoAdjustCameraClippingRange = value;
  this->JoystickActor->SetAutoAdjustCameraClippingRange(value);
  this->JoystickCamera->SetAutoAdjustCameraClippingRange(value);
  this->TrackballActor->SetAutoAdjustCameraClippingRange(value);
  this->TrackballCamera->SetAutoAdjustCameraClippingRange(value);
  this->MultiTouchCamera->SetAutoAdjustCameraClippingRange(value);

  this->Modified();
}

//------------------------------------------------------------------------------
void vtkInteractorStyleSwitch::SetCurrentStyleToJoystickActor()
{
  this->JoystickOrTrackball = VTKIS_JOYSTICK;
  this->CameraOrActor = VTKIS_ACTOR;
  this->MultiTouch = false;
  this->SetCurrentStyle();
}

//------------------------------------------------------------------------------
void vtkInteractorStyleSwitch::SetCurrentStyleToJoystickCamera()
{
  this->JoystickOrTrackball = VTKIS_JOYSTICK;
  this->CameraOrActor = VTKIS_CAMERA;
  this->MultiTouch = false;
  this->SetCurrentStyle();
}

//------------------------------------------------------------------------------
void vtkInteractorStyleSwitch::SetCurrentStyleToTrackballActor()
{
  this->JoystickOrTrackball = VTKIS_TRACKBALL;
  this->CameraOrActor = VTKIS_ACTOR;
  this->MultiTouch = false;
  this->SetCurrentStyle();
}

//------------------------------------------------------------------------------
void vtkInteractorStyleSwitch::SetCurrentStyleToTrackballCamera()
{
  this->JoystickOrTrackball = VTKIS_TRACKBALL;
  this->CameraOrActor = VTKIS_CAMERA;
  this->MultiTouch = false;
  this->SetCurrentStyle();
}

//------------------------------------------------------------------------------
void vtkInteractorStyleSwitch::SetCurrentStyleToMultiTouchCamera()
{
  this->MultiTouch = true;
  this->SetCurrentStyle();
}

//------------------------------------------------------------------------------
void vtkInteractorStyleSwitch::OnChar()
{
  switch (this->Interactor->GetKeyCode())
  {
    case 'j':
    case 'J':
      this->JoystickOrTrackball = VTKIS_JOYSTICK;
      this->MultiTouch = false;
      this->EventCallbackCommand->SetAbortFlag(1);
      break;
    case 't':
    case 'T':
      this->JoystickOrTrackball = VTKIS_TRACKBALL;
      this->MultiTouch = false;
      this->EventCallbackCommand->SetAbortFlag(1);
      break;
    case 'c':
    case 'C':
      this->CameraOrActor = VTKIS_CAMERA;
      this->MultiTouch = false;
      this->EventCallbackCommand->SetAbortFlag(1);
      break;
    case 'a':
    case 'A':
      this->CameraOrActor = VTKIS_ACTOR;
      this->MultiTouch = false;
      this->EventCallbackCommand->SetAbortFlag(1);
      break;
    case 'm':
    case 'M':
      this->MultiTouch = true;
      this->EventCallbackCommand->SetAbortFlag(1);
      break;
  }
  // Set the CurrentStyle pointer to the picked style
  this->SetCurrentStyle();
}

//------------------------------------------------------------------------------
// this will do nothing if the CurrentStyle matches
// JoystickOrTrackball and CameraOrActor
// It should! If the this->Interactor was changed (using SetInteractor()),
// and the currentstyle should not change.
void vtkInteractorStyleSwitch::SetCurrentStyle()
{
  // if the currentstyle does not match JoystickOrTrackball
  // and CameraOrActor ivars, then call SetInteractor(0)
  // on the Currentstyle to remove all of the observers.
  // Then set the Currentstyle and call SetInteractor with
  // this->Interactor so the callbacks are set for the
  // currentstyle.
  if (this->MultiTouch)
  {
    if (this->CurrentStyle != this->MultiTouchCamera)
    {
      if (this->CurrentStyle)
      {
        this->CurrentStyle->SetInteractor(nullptr);
      }
      this->CurrentStyle = this->MultiTouchCamera;
    }
  }
  else if (this->JoystickOrTrackball == VTKIS_JOYSTICK && this->CameraOrActor == VTKIS_CAMERA)
  {
    if (this->CurrentStyle != this->JoystickCamera)
    {
      if (this->CurrentStyle)
      {
        this->CurrentStyle->SetInteractor(nullptr);
      }
      this->CurrentStyle = this->JoystickCamera;
    }
  }
  else if (this->JoystickOrTrackball == VTKIS_JOYSTICK && this->CameraOrActor == VTKIS_ACTOR)
  {
    if (this->CurrentStyle != this->JoystickActor)
    {
      if (this->CurrentStyle)
      {
        this->CurrentStyle->SetInteractor(nullptr);
      }
      this->CurrentStyle = this->JoystickActor;
    }
  }
  else if (this->JoystickOrTrackball == VTKIS_TRACKBALL && this->CameraOrActor == VTKIS_CAMERA)
  {
    if (this->CurrentStyle != this->TrackballCamera)
    {
      if (this->CurrentStyle)
      {
        this->CurrentStyle->SetInteractor(nullptr);
      }
      this->CurrentStyle = this->TrackballCamera;
    }
  }
  else if (this->JoystickOrTrackball == VTKIS_TRACKBALL && this->CameraOrActor == VTKIS_ACTOR)
  {
    if (this->CurrentStyle != this->TrackballActor)
    {
      if (this->CurrentStyle)
      {
        this->CurrentStyle->SetInteractor(nullptr);
      }
      this->CurrentStyle = this->TrackballActor;
    }
  }
  if (this->CurrentStyle)
  {
    this->CurrentStyle->SetInteractor(this->Interactor);
    this->CurrentStyle->SetTDxStyle(this->TDxStyle);
  }
}

//------------------------------------------------------------------------------
void vtkInteractorStyleSwitch::SetInteractor(vtkRenderWindowInteractor* iren)
{
  if (iren == this->Interactor)
  {
    return;
  }
  // if we already have an Interactor then stop observing it
  if (this->Interactor)
  {
    this->Interactor->RemoveObserver(this->EventCallbackCommand);
  }
  this->Interactor = iren;
  // add observers for each of the events handled in ProcessEvents
  if (iren)
  {
    iren->AddObserver(vtkCommand::CharEvent, this->EventCallbackCommand, this->Priority);

    iren->AddObserver(vtkCommand::DeleteEvent, this->EventCallbackCommand, this->Priority);
  }
  this->SetCurrentStyle();
}

//------------------------------------------------------------------------------
void vtkInteractorStyleSwitch::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "CurrentStyle " << this->CurrentStyle << "\n";
  if (this->CurrentStyle)
  {
    vtkIndent next_indent = indent.GetNextIndent();
    os << next_indent << this->CurrentStyle->GetClassName() << "\n";
    this->CurrentStyle->PrintSelf(os, indent.GetNextIndent());
  }
}

//------------------------------------------------------------------------------
void vtkInteractorStyleSwitch::SetDefaultRenderer(vtkRenderer* renderer)
{
  this->vtkInteractorStyle::SetDefaultRenderer(renderer);
  this->JoystickActor->SetDefaultRenderer(renderer);
  this->JoystickCamera->SetDefaultRenderer(renderer);
  this->TrackballActor->SetDefaultRenderer(renderer);
  this->TrackballCamera->SetDefaultRenderer(renderer);
}

//------------------------------------------------------------------------------
void vtkInteractorStyleSwitch::SetCurrentRenderer(vtkRenderer* renderer)
{
  this->vtkInteractorStyle::SetCurrentRenderer(renderer);
  this->JoystickActor->SetCurrentRenderer(renderer);
  this->JoystickCamera->SetCurrentRenderer(renderer);
  this->TrackballActor->SetCurrentRenderer(renderer);
  this->TrackballCamera->SetCurrentRenderer(renderer);
}
VTK_ABI_NAMESPACE_END
