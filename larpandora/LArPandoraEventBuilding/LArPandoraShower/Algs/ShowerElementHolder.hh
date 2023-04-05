//###################################################################
//### Name:        ShowerElementHolder                            ###
//### Author:      Dominic Barker, Ed Tyley                       ###
//### Date:        15.07.19                                       ###
//### Description: Class to holder the standard shower property   ###
//###              information. Used in LArPandoraModularShower   ###
//###              and corresponding tools                        ###
//###################################################################

#ifndef ShowerElementHolder_HH
#define ShowerElementHolder_HH

//Framework includes
#include "art/Framework/Principal/Handle.h"
#include "canvas/Persistency/Common/FindManyP.h"
#include "canvas/Persistency/Common/FindOneP.h"
#include "cetlib_except/demangle.h"
#include "cetlib_except/exception.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

//C++ Inlcudes
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <string>

namespace reco::shower {
  class ShowerElementBase;
  template <class T>
  class ShowerElementAccessor;
  template <class T>
  class ShowerDataProduct;
  template <class T>
  class EventDataProduct;
  template <class T, class T2>
  class ShowerProperty;
  class ShowerElementHolder;
}

class reco::shower::ShowerElementBase {

public:
  virtual ~ShowerElementBase() noexcept = default;

  virtual bool CheckTag() const
  {
    throw cet::exception("ShowerElementHolder")
      << "Trying to check an element that is not a product" << std::endl;
  }
  virtual void SetCheckTag(bool& check)
  {
    throw cet::exception("ShowerElementHolder")
      << "Trying to set an element that is not a product" << std::endl;
  }

  virtual std::string GetType() const = 0;

  //Check if the element has been set.
  bool CheckShowerElement() const
  {
    if (elementPtr)
      return true;
    else
      return false;
  }

  void Clear() { elementPtr = 0; }

protected:
  bool elementPtr;
};

//This is a template class which holds a shower property. This holds any object e.g. std::vector<double>, double, TVector3
//and holds various information which helps the showerproperty holder access the elements. A user should not require any part
//of this class.
template <class T>
class reco::shower::ShowerElementAccessor : public reco::shower::ShowerElementBase {

public:
  ShowerElementAccessor(T& Element) : element(Element)
  {
    this->elementPtr = 1;
    // this->element         = Element;
  }

  //Set the element in the holder
  void SetShowerElement(T& Element)
  {
    element = Element;
    this->elementPtr = 1;
  }

  //Fill Element with the element that the holder holds.
  int GetShowerElement(T& Element) const
  {
    if (this->elementPtr) {
      Element = element;
      return 0;
    }
    else {
      return 1;
    }
  }

  //Return a copy of the shower element.
  T& GetShowerElementRef()
  {
    if (!this->elementPtr) {
      throw cet::exception("ShowerElementHolder")
        << "The element that is being accessed is not set" << std::endl;
    }
    return element;
  }

  T GetShowerElement() const
  {
    if (!this->elementPtr) {
      throw cet::exception("ShowerElementHolder")
        << "The element that is being accessed is not set" << std::endl;
    }
    return element;
  }

  //Return the type as a string.
  std::string GetType() const override { return cet::demangle_symbol(typeid(element).name()); }

protected:
  T element;
};

//This class holds shower data products which have the potential to be saved in the art::Event e.g. recob::Track. Note the product itself must be store in the element holder as the object will be destoryed in the CalculateProperty Section otherwise. Associtations can be made during Calculate property tool stage.
template <class T>
class reco::shower::ShowerDataProduct : public reco::shower::ShowerElementAccessor<T> {

public:
  ShowerDataProduct(T& Element, bool Checktag) : reco::shower::ShowerElementAccessor<T>{Element}
  {
    checktag = Checktag;
  }

  void Clear()
  {
    this->element = T();
    this->elementPtr = 0;
  }

  //Check if we should check the dataproduct in the end.
  bool CheckTag() const { return checktag; }

  //Set if we should check the data product in the end.
  void SetCheckTag(bool& Checktag) { checktag = Checktag; }

private:
  bool checktag;
};

// This class holds the things we want per event rather than per shower, e.g. FindManyP
template <class T>
class reco::shower::EventDataProduct : public reco::shower::ShowerElementAccessor<T> {

public:
  EventDataProduct(T& Element) : reco::shower::ShowerElementAccessor<T>{Element} {}

  void Clear()
  {
    // this->element    = T();
    this->elementPtr = 0;
  }
};

//This class holds shower properties e.g. ShowerDirection. The user must define the associated error
template <class T, class T2>
class reco::shower::ShowerProperty : public reco::shower::ShowerElementAccessor<T> {

public:
  ShowerProperty(T& Element, T2& ElementErr) : reco::shower::ShowerElementAccessor<T>{Element}
  {
    propertyErr = ElementErr;
  }

  //Fill the property error as long as it has been set.
  int GetShowerPropertyError(T2& ElementErr) const
  {
    if (this->elementPtr) {
      ElementErr = propertyErr;
      return 0;
    }
    else {
      return 1;
    }
  }

  //Set the properties. Note you cannot set an property without an error.
  void SetShowerProperty(T& Element, T2& ElementErr)
  {
    this->element = Element;
    this->elementPtr = 1;
    propertyErr = ElementErr;
  }

  void Clear()
  {
    this->element = T();
    this->elementPtr = 0;
  }

private:
  T2 propertyErr;
};

//Class to holder all the reco::shower::ShowerElement objects. This is essentially a map from a string the object so people can
//add an object in a tool and get it back later.
class reco::shower::ShowerElementHolder {

public:
  //Getter function for accessing the shower property e..g the direction ShowerElementHolder.GetElement("MyShowerValue"); The name is used access the value and precise names are required for a complete shower in LArPandoraModularShowerCreation: ShowerStartPosition, ShowerDirection, ShowerEnergy ,ShowerdEdx.
  template <class T>
  int GetElement(const std::string& Name, T& Element) const
  {
    auto const showerPropertiesIt = showerproperties.find(Name);
    if (showerPropertiesIt != showerproperties.end()) {
      if (showerPropertiesIt->second->CheckShowerElement()) {
        reco::shower::ShowerElementAccessor<T>* showerprop =
          dynamic_cast<reco::shower::ShowerElementAccessor<T>*>(showerPropertiesIt->second.get());
        if (showerprop == nullptr) {
          throw cet::exception("ShowerElementHolder")
            << "Trying to get Element: " << Name
            << ". This element you are filling is not the correct type" << std::endl;
        }
        showerprop->GetShowerElement(Element);
        return 0;
      }
      else {
        mf::LogWarning("ShowerElementHolder")
          << "Trying to get Element " << Name << ". This elment has not been filled" << std::endl;
        return 1;
      }
    }

    auto const showerDataProductsIt = showerdataproducts.find(Name);
    if (showerDataProductsIt != showerdataproducts.end()) {
      if (showerDataProductsIt->second->CheckShowerElement()) {
        reco::shower::ShowerElementAccessor<T>* showerprop =
          dynamic_cast<reco::shower::ShowerElementAccessor<T>*>(showerDataProductsIt->second.get());
        if (showerprop == nullptr) {
          throw cet::exception("ShowerElementHolder")
            << "Trying to get Element: " << Name
            << ". This element you are filling is not the correct type" << std::endl;
        }
        showerprop->GetShowerElement(Element);
        return 0;
      }
      else {
        mf::LogWarning("ShowerElementHolder")
          << "Trying to get Element " << Name << ". This elment has not been filled" << std::endl;
        return 1;
      }
    }

    auto const eventDataProductsIt = eventdataproducts.find(Name);
    if (eventDataProductsIt != eventdataproducts.end()) {
      if (eventDataProductsIt->second->CheckShowerElement()) {
        reco::shower::ShowerElementAccessor<T>* eventprop =
          dynamic_cast<reco::shower::ShowerElementAccessor<T>*>(eventDataProductsIt->second.get());
        if (eventprop == nullptr) {
          throw cet::exception("ShowerElementHolder")
            << "Trying to get Element: " << Name
            << ". This element you are filling is not the correct type" << std::endl;
        }
        eventprop->GetShowerElement(Element);
        return 0;
      }
      else {
        mf::LogWarning("ShowerElementHolder")
          << "Trying to get Element " << Name << ". This elment has not been filled" << std::endl;
        return 1;
      }
    }
    throw cet::exception("ShowerElementHolder")
      << "Trying to get Element: " << Name << ". This element does not exist in the element holder"
      << std::endl;
  }

  template <class T>
  int GetEventElement(const std::string& Name, T& Element) const
  {
    auto const eventDataProductsIt = eventdataproducts.find(Name);
    if (eventDataProductsIt != eventdataproducts.end()) {
      if (eventDataProductsIt->second->CheckShowerElement()) {
        reco::shower::ShowerElementAccessor<T>* eventprop =
          dynamic_cast<reco::shower::ShowerElementAccessor<T>*>(eventDataProductsIt->second.get());
        if (eventprop == nullptr) {
          throw cet::exception("ShowerElementHolder")
            << "Trying to get Element: " << Name
            << ". This element you are filling is not the correct type" << std::endl;
        }
        eventprop->GetShowerElement(Element);
        return 0;
      }
      else {
        mf::LogWarning("ShowerElementHolder")
          << "Trying to get Element " << Name << ". This elment has not been filled" << std::endl;
        return 1;
      }
    }
    throw cet::exception("ShowerElementHolder")
      << "Trying to get Element: " << Name << ". This element does not exist in the element holder"
      << std::endl;
  }

  //Alternative get function that returns the object. Not recommended.
  template <class T>
  const T& GetEventElement(std::string const& Name)
  {
    auto const eventDataProductsIt = eventdataproducts.find(Name);
    if (eventDataProductsIt != eventdataproducts.end()) {
      if (eventDataProductsIt->second->CheckShowerElement()) {
        reco::shower::ShowerElementAccessor<T>* eventprop =
          dynamic_cast<reco::shower::ShowerElementAccessor<T>*>(eventDataProductsIt->second.get());
        if (eventprop == nullptr) {
          throw cet::exception("ShowerElementHolder")
            << "Trying to get Element: " << Name
            << ". This element you are filling is not the correct type" << std::endl;
        }
        return eventprop->GetShowerElementRef();
      }
    }
    throw cet::exception("ShowerElementHolder")
      << "Trying to get Element: " << Name << ". This element does not exist in the element holder"
      << std::endl;
  }

  //Alternative get function that returns the object. Not recommended.
  template <class T>
  T GetElement(const std::string& Name) const
  {
    auto const showerPropertiesIt = showerproperties.find(Name);
    if (showerPropertiesIt != showerproperties.end()) {
      if (showerPropertiesIt->second->CheckShowerElement()) {
        reco::shower::ShowerElementAccessor<T>* showerprop =
          dynamic_cast<reco::shower::ShowerElementAccessor<T>*>(showerPropertiesIt->second.get());
        if (showerprop == nullptr) {
          throw cet::exception("ShowerElementHolder")
            << "Trying to get Element: " << Name
            << ". This element you are filling is not the correct type" << std::endl;
        }
        return showerprop->GetShowerElement();
      }
    }

    auto const showerDataProductsIt = showerdataproducts.find(Name);
    if (showerDataProductsIt != showerdataproducts.end()) {
      if (showerDataProductsIt->second->CheckShowerElement()) {
        reco::shower::ShowerElementAccessor<T>* showerprop =
          dynamic_cast<reco::shower::ShowerElementAccessor<T>*>(showerDataProductsIt->second.get());
        if (showerprop == nullptr) {
          throw cet::exception("ShowerElementHolder")
            << "Trying to get Element: " << Name
            << ". This element you are filling is not the correct type" << std::endl;
        }
        return showerprop->GetShowerElement();
      }
    }

    auto const eventDataProductsIt = eventdataproducts.find(Name);
    if (eventDataProductsIt != eventdataproducts.end()) {
      if (eventDataProductsIt->second->CheckShowerElement()) {
        reco::shower::ShowerElementAccessor<T>* eventprop =
          dynamic_cast<reco::shower::ShowerElementAccessor<T>*>(eventDataProductsIt->second.get());
        if (eventprop == nullptr) {
          throw cet::exception("ShowerElementHolder")
            << "Trying to get Element: " << Name
            << ". This element you are filling is not the correct type" << std::endl;
        }
        return eventprop->GetShowerElement();
      }
    }
    throw cet::exception("ShowerElementHolder")
      << "Trying to get Element: " << Name << ". This element does not exist in the element holder"
      << std::endl;
  }

  //Getter function for accessing the shower property error e.g the direction ShowerElementHolder.GetElement("MyShowerValue");
  template <class T, class T2>
  int GetElementAndError(const std::string& Name, T& Element, T2& ElementErr) const
  {
    auto const showerPropertiesIt = showerproperties.find(Name);
    if (showerPropertiesIt == showerproperties.end()) {
      mf::LogError("ShowerElementHolder")
        << "Trying to get Element Error: " << Name
        << ". This elment does not exist in the element holder" << std::endl;
      return 1;
    }
    reco::shower::ShowerProperty<T, T2>* showerprop =
      dynamic_cast<reco::shower::ShowerProperty<T, T2>*>(showerPropertiesIt->second.get());
    showerprop->GetShowerElement(Element);
    showerprop->GetShowerPropertyError(ElementErr);
    return 0;
  }

  //This sets the value of the data product. Just give a name and a object
  //e.g. TVector3 ShowerElementHolder.SetElement((TVector3) StartPosition, "StartPosition");
  template <class T>
  void SetElement(T& dataproduct, const std::string& Name, bool checktag = false)
  {

    auto const showerDataProductsIt = showerdataproducts.find(Name);
    if (showerDataProductsIt != showerdataproducts.end()) {
      reco::shower::ShowerDataProduct<T>* showerdataprod =
        dynamic_cast<reco::shower::ShowerDataProduct<T>*>(showerDataProductsIt->second.get());
      showerdataprod->SetShowerElement(dataproduct);
      showerdataprod->SetCheckTag(checktag);
      return;
    }
    else {
      showerdataproducts[Name] = std::make_unique<ShowerDataProduct<T>>(dataproduct, checktag);
      return;
    }
  }

  //This sets the value of the property. Just give a name and a object
  //e.g. TVector3 ShowerElementHolder.SetElement((art::Ptr<recob::Track>) track, "StartPosition", save);
  template <class T, class T2>
  void SetElement(T& propertyval, T2& propertyvalerror, const std::string& Name)
  {

    auto const showerPropertiesIt = showerproperties.find(Name);
    if (showerPropertiesIt != showerproperties.end()) {
      reco::shower::ShowerProperty<T, T2>* showerprop =
        dynamic_cast<reco::shower::ShowerProperty<T, T2>*>(showerPropertiesIt->second.get());
      showerprop->SetShowerProperty(propertyval, propertyvalerror);
      return;
    }
    else {
      showerproperties[Name] =
        std::make_unique<ShowerProperty<T, T2>>(propertyval, propertyvalerror);
      return;
    }
  }

  //This sets the value of the event data product. Just give a name and a object
  //e.g. TVector3 ShowerElementHolder.SetEventElement((TVector3) StartPosition, "StartPosition");
  template <class T>
  void SetEventElement(T& dataproduct, const std::string& Name)
  {

    auto const eventDataProductsIt = eventdataproducts.find(Name);
    if (eventDataProductsIt != eventdataproducts.end()) {
      reco::shower::EventDataProduct<T>* eventdataprod =
        dynamic_cast<reco::shower::EventDataProduct<T>*>(eventDataProductsIt->second.get());
      eventdataprod->SetShowerElement(dataproduct);
      return;
    }
    else {
      eventdataproducts[Name] = std::make_unique<EventDataProduct<T>>(dataproduct);
      return;
    }
  }

  bool CheckEventElement(const std::string& Name) const
  {
    auto const eventDataProductsIt = eventdataproducts.find(Name);
    return eventDataProductsIt == eventdataproducts.end() ?
             false :
             eventDataProductsIt->second->CheckShowerElement();
  }

  //Check that a property is filled
  bool CheckElement(const std::string& Name) const
  {
    auto const showerPropertiesIt = showerproperties.find(Name);
    if (showerPropertiesIt != showerproperties.end()) {
      return showerPropertiesIt->second->CheckShowerElement();
    }
    auto const showerDataProductsIt = showerdataproducts.find(Name);
    if (showerDataProductsIt != showerdataproducts.end()) {
      return showerDataProductsIt->second->CheckShowerElement();
    }
    auto const eventDataProductsIt = eventdataproducts.find(Name);
    if (eventDataProductsIt != eventdataproducts.end()) {
      return eventDataProductsIt->second->CheckShowerElement();
    }
    return false;
  }

  //Check All the properties
  bool CheckAllElements() const
  {
    bool checked = true;
    for (auto const& showerprop : showerproperties) {
      checked *= showerprop.second->CheckShowerElement();
    }
    for (auto const& showerdataprod : showerdataproducts) {
      checked *= showerdataprod.second->CheckShowerElement();
    }
    return checked;
  }

  //Clear Fucntion. This does not delete the element.
  void ClearElement(const std::string& Name)
  {
    auto const showerPropertiesIt = showerproperties.find(Name);
    if (showerPropertiesIt != showerproperties.end()) {
      return showerPropertiesIt->second->Clear();
    }
    auto const showerDataProductsIt = showerdataproducts.find(Name);
    if (showerDataProductsIt != showerdataproducts.end()) {
      return showerDataProductsIt->second->Clear();
    }
    mf::LogError("ShowerElementHolder")
      << "Trying to clear Element: " << Name
      << ". This element does not exist in the element holder" << std::endl;
    return;
  }

  //Clear all the shower properties. This does not delete the element.
  void ClearShower()
  {
    for (auto const& showerprop : showerproperties) {
      (showerprop.second)->Clear();
    }
    for (auto const& showerdataproduct : showerdataproducts) {
      (showerdataproduct.second)->Clear();
    }
  }
  //Clear all the shower properties. This does not delete the element.
  void ClearEvent()
  {
    for (auto const& eventdataproduct : eventdataproducts) {
      (eventdataproduct.second)->Clear();
    }
  }
  //Clear all the shower properties. This does not delete the element.
  void ClearAll()
  {
    ClearShower();
    ClearEvent();
  }

  //Find if the product is one what is being stored.
  bool CheckElementTag(const std::string& Name) const
  {
    auto const showerDataProductsIt = showerdataproducts.find(Name);
    if (showerDataProductsIt != showerdataproducts.end()) {
      return showerDataProductsIt->second->CheckTag();
    }
    return false;
  }

  //Delete a product. I see no reason for it.
  void DeleteElement(const std::string& Name)
  {
    auto const showerPropertiesIt = showerproperties.find(Name);
    if (showerPropertiesIt != showerproperties.end()) {
      return showerPropertiesIt->second.reset(nullptr);
    }
    auto const showerDataProductsIt = showerdataproducts.find(Name);
    if (showerDataProductsIt != showerdataproducts.end()) {
      return showerDataProductsIt->second.reset(nullptr);
    }
    mf::LogError("ShowerElementHolder")
      << "Trying to delete Element: " << Name
      << ". This element does not exist in the element holder" << std::endl;
    return;
  }

  //Set the indicator saying if the shower is going to be stored.
  void SetElementTag(const std::string& Name, bool checkelement)
  {
    auto const showerDataProductsIt = showerdataproducts.find(Name);
    if (showerDataProductsIt != showerdataproducts.end()) {
      return showerDataProductsIt->second->SetCheckTag(checkelement);
    }
    mf::LogError("ShowerElementHolder")
      << "Trying set the checking of the data product: " << Name
      << ". This data product does not exist in the element holder" << std::endl;
    return;
  }

  bool CheckAllElementTags() const
  {
    bool checked = true;
    for (auto const& showerdataproduct : showerdataproducts) {
      bool check = showerdataproduct.second->CheckTag();
      if (check) {
        bool elementset = showerdataproduct.second->CheckShowerElement();
        if (!elementset) {
          mf::LogError("ShowerElementHolder")
            << "The following element is not set and was asked to be checked: "
            << showerdataproduct.first << std::endl;
          checked = false;
        }
      }
    }
    return checked;
  }

  //Set the shower number. This is required the association making.
  void SetShowerNumber(int& shower_iter) { showernumber = shower_iter; }

  //Get the shower number.
  int GetShowerNumber() const { return showernumber; }

  //This function will print out all the elements and there types for the user to check.
  void PrintElements() const
  {

    unsigned int maxname = 0;
    for (auto const& showerprop : showerproperties) {
      if (showerprop.first.size() > maxname) { maxname = showerprop.first.size(); }
    }
    for (auto const& showerdataprod : showerdataproducts) {
      if (showerdataprod.first.size() > maxname) { maxname = showerdataprod.first.size(); }
    }

    std::map<std::string, std::string> Type_showerprops;
    std::map<std::string, std::string> Type_showerdataprods;
    for (auto const& showerprop : showerproperties) {
      std::string Type = (showerprop.second)->GetType();
      Type_showerprops[showerprop.first] = Type;
    }
    for (auto const& showerdataprod : showerdataproducts) {
      std::string Type = (showerdataprod.second)->GetType();
      Type_showerdataprods[showerdataprod.first] = Type;
    }

    unsigned int maxtype = 0;
    for (auto const& Type_showerprop : Type_showerprops) {
      if (Type_showerprop.second.size() > maxtype) { maxtype = Type_showerprop.second.size(); }
    }
    for (auto const& Type_showerdataprod : Type_showerdataprods) {
      if (Type_showerdataprod.second.size() > maxtype) {
        maxtype = Type_showerdataprod.second.size();
      }
    }

    unsigned int n = maxname + maxtype + 33;
    std::cout << std::left << std::setfill('*') << std::setw(n - 1) << "*" << std::endl;
    std::cout << "Elements in the element holder" << std::endl;
    std::cout << std::left << std::setfill('*') << std::setw(n - 1) << "*" << std::endl;
    for (auto const& Type_showerprop : Type_showerprops) {
      std::cout << std::left << std::setfill(' ') << std::setw(21)
                << "* Property Name: " << std::setw(maxname) << Type_showerprop.first;
      std::cout << std::left << std::setfill(' ') << " * Type: " << std::setw(maxtype)
                << Type_showerprop.second << " * " << std::endl;
    }
    for (auto const& Type_showerdataprod : Type_showerdataprods) {
      std::cout << std::left << std::setfill(' ') << std::setw(maxname) << std::setw(21)
                << "* Data Product Name: " << std::setw(maxname) << Type_showerdataprod.first;
      std::cout << std::left << std::setfill(' ') << " * Type: " << std::setw(maxtype)
                << Type_showerdataprod.second << " *" << std::endl;
    }
    std::cout << std::left << std::setfill('*') << std::setw(n - 1) << "*" << std::endl;
    std::cout << std::setfill(' ');
    std::cout << std::setw(0);
    return;
  }

  template <class T>
  std::string getType(T object) const
  {
    return cet::demangle_symbol(typeid(object).name());
  }

  template <class T>
  std::string getType() const
  {
    return cet::demangle_symbol(typeid(T).name());
  }

  template <class T1, class T2>
  const art::FindManyP<T1>& GetFindManyP(const art::ValidHandle<std::vector<T2>>& handle,
                                         const art::Event& evt,
                                         const art::InputTag& moduleTag)
  {

    const std::string name("FMP_" + moduleTag.label() + "_" + getType<T1>() + "_" + getType<T2>());

    if (CheckEventElement(name)) { return GetEventElement<art::FindManyP<T1>>(name); }
    else {
      art::FindManyP<T1> findManyP(handle, evt, moduleTag);
      if (findManyP.isValid()) {
        SetEventElement(findManyP, name);
        return GetEventElement<art::FindManyP<T1>>(name);
      }
      else {
        throw cet::exception("ShowerElementHolder")
          << "FindManyP is not valid: " << name << std::endl;
      }
    }
  }

  template <class T1, class T2>
  const art::FindOneP<T1>& GetFindOneP(const art::ValidHandle<std::vector<T2>>& handle,
                                       const art::Event& evt,
                                       const art::InputTag& moduleTag)
  {

    const std::string name("FOP_" + moduleTag.label() + "_" + getType<T1>() + "_" + getType<T2>());

    if (CheckEventElement(name)) { return GetEventElement<art::FindOneP<T1>>(name); }
    else {
      art::FindOneP<T1> findOneP(handle, evt, moduleTag);
      if (findOneP.isValid()) {
        SetEventElement(findOneP, name);
        return GetEventElement<art::FindOneP<T1>>(name);
      }
      else {
        throw cet::exception("ShowerElementHolder")
          << "FindOneP is not valid: " << name << std::endl;
      }
    }
  }

private:
  //Storage for all the shower properties.
  std::map<std::string, std::unique_ptr<reco::shower::ShowerElementBase>> showerproperties;

  //Storage for all the data products
  std::map<std::string, std::unique_ptr<reco::shower::ShowerElementBase>> showerdataproducts;

  //Storage for all the data products
  std::map<std::string, std::unique_ptr<reco::shower::ShowerElementBase>> eventdataproducts;

  //Shower ID number. Use this to set ptr makers.
  int showernumber;
};

#endif
