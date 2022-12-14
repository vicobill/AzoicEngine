// Copyright 2005-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

struct IVehicleData;

XmlNodeRef VehicleDataSave(const char* definitionFile, IVehicleData* pData);
bool       VehicleDataSave(const char* definitionFile, const char* dataFile, IVehicleData* pData);

// This Save method merges the vehicle data using the original source xml
// without losing data that is unknown to the vehicle definition
XmlNodeRef VehicleDataMergeAndSave(const char* originalXml, XmlNodeRef definition, IVehicleData* pData);
