// -*- c-basic-offset: 4 -*-

/** @file ProjectListBox.cpp
 *
 *  @brief Batch processor for Hugin with GUI
 *
 *  @author Marko Kuder <marko.kuder@gmail.com>
 *
 *  $Id: ProjectListBox.cpp 3322 2008-08-16 5:00:07Z mkuder $
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this software; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "ProjectListBox.h"

BEGIN_EVENT_TABLE(ProjectListBox, wxListCtrl)
 EVT_LIST_ITEM_SELECTED(wxID_ANY, ProjectListBox::OnSelect)
 EVT_LIST_ITEM_DESELECTED(wxID_ANY, ProjectListBox::OnDeselect)
 EVT_LIST_COL_END_DRAG(wxID_ANY, ProjectListBox::OnColumnWidthChange)
END_EVENT_TABLE()

ProjectListBox::ProjectListBox(wxWindow* parent, wxWindowID id, wxPoint point, wxSize size, long style) : wxListCtrl(parent, id, point, size, style)
{
	columns.Add(ID),
	columns.Add(PROJECT);
	columns.Add(PREFIX);
	columns.Add(STATUS);
	columns.Add(MODDATE);
	columns.Add(FORMAT);
	columns.Add(PROJECTION);
	columns.Add(SIZE);

	m_selected = -1;
	for(unsigned int i=0; i<columns.GetCount(); i++)
		this->InsertColumn(i,columnTitle[columns.Item(i)]);
	/*this->InsertColumn(0,_("ID"));
	this->InsertColumn(1,_("Project"));
	this->InsertColumn(2,_("Output prefix"));
	this->InsertColumn(3,_("Last modified"));
	this->InsertColumn(4,_("Output format"));
	this->InsertColumn(5,_("Projection"));
	this->InsertColumn(6,_("Size"));*/

	//get saved width
    for( int i=0; i < GetColumnCount() ; i++ )
    {
        int width = wxConfigBase::Get()->Read(wxString::Format(wxT("/BatchList/ColumnWidth%d"), columns[i] ), -1);
        if(width != -1)
            SetColumnWidth(i, width);
    }
}

//public methods:

void ProjectListBox::AppendProject(Project* project)
{
	/*//already included
	//path
	//prefix
	//modified
	project->options.getOutputExtension() //std string
	project->options.getFormatName( //std string
	project->options.getProjection() //enum projectionFormat
	project->options.getWidth()
	project->options.getHeight()*/

	/*//other possible columns
		project->options.enblendOptions  //std string
		project->options.enfuseOptions   //std string
		project->options.outputMode  //enum outputMode
		project->options.featherWidth
		project->options.gamma	
		project->options.getHFOV()
		project->options.getVFOV()
		project->options.hdrMergeMode //enum HDRMergeType
		project->options.colorCorrection
		project->options.hdrmergeOptions //std string
		project->options.quality
		project->options.blendMode  //enum blendingMechanism*/
	if(project->id < 0) //if we have a command-line application
	{
		int i=columns.Index(PROJECT);	//we find the project column
		if(i != wxNOT_FOUND)
		{
			if(i==0)
				this->InsertItem(this->GetItemCount(),project->path);
			else
			{
				this->InsertItem(this->GetItemCount(),_T(""));
				this->SetItem(this->GetItemCount()-1,i,project->path);
			}
		}
		else	//we insert an empty line
			this->InsertItem(this->GetItemCount(),_T(""));
	}
	else
	{
		if(columns.GetCount()>0)
		{
			this->InsertItem(this->GetItemCount(),this->GetAttributeString(columns[0],project));
			for(unsigned int i=1; i<columns.GetCount(); i++)
				this->SetItem(this->GetItemCount()-1,i,this->GetAttributeString(columns[i],project));
		}
		else	//we have no columns?
			this->InsertItem(this->GetItemCount(),_T(""));
	}
}

void ProjectListBox::ChangePrefix(int index, wxString newPrefix)
{
	if(columns.Index(PREFIX)!=wxNOT_FOUND)
		this->SetItem(index,columns.Index(PREFIX),newPrefix);
}

void ProjectListBox::Deselect(int index)
{
	SetItemState(index, 0, wxLIST_STATE_SELECTED|wxLIST_STATE_FOCUSED);
}

void ProjectListBox::Fill(Batch* batch)
{
	m_batch=batch;
	for(int i=0; i<m_batch->GetProjectCount(); i++)
		AppendProject(m_batch->GetProject(i));
}

int ProjectListBox::GetIndex(int id)
{
	int index=0;
	while(index<this->GetItemCount())
	{
		if(GetText(index,0).Cmp(wxString::Format(_("%d"),id))==0)
			return index;
		index++;
	}
	return -1;
}

int ProjectListBox::GetProjectCountByPath(wxString path)
{
	int count = 0;
	for(int i=0; i<this->GetItemCount(); i++)
	{
		//wxMessageBox( path,GetText(i,0),wxOK | wxICON_INFORMATION );
		if(path.Cmp(GetText(i,1))==0)
			count++;
	}
	return count;
}

int ProjectListBox::GetProjectId(int index)
{
	long id=-1;
	if(!GetText(index,0).ToLong(&id))
		wxMessageBox(_T("Error, cannot convert id"),_T("Error"));
	return (int)id;
}
int ProjectListBox::GetSelectedIndex()
{
	return m_selected;
}

wxString ProjectListBox::GetSelectedProject()
{
	return GetText(m_selected,1);
}

wxString ProjectListBox::GetText(int row, int column)
{
	wxListItem item;
	item.SetId(row);
	item.SetColumn(column);
	item.SetMask(wxLIST_MASK_TEXT);
	this->GetItem(item);
	return item.GetText();
}

void ProjectListBox::ReloadProject(int index, Project* project)
{
	for(unsigned int i=0; i<columns.GetCount(); i++)
		this->SetItem(index,i,this->GetAttributeString(columns[i],project));
}

void ProjectListBox::Select(int index)
{
	SetItemState(index,wxLIST_STATE_SELECTED,wxLIST_STATE_SELECTED);
}

void ProjectListBox::SetMissing(int index)
{
	for(int i=0; i< this->GetColumnCount(); i++)
	{
		if(columns[i]==STATUS)
			this->SetItem(index,i,_T("File missing"));
		if(columns[i]!=ID && columns[i]!=PROJECT && columns[i]!=PREFIX)
			this->SetItem(index,i,_T(""));
	}
}

void ProjectListBox::SwapProject(int index)
{
	wxString temp;
	for(int i=0; i<GetColumnCount(); i++)
	{
		temp = GetText(index,i);
		SetItem(index,i,GetText(index+1,i));
		SetItem(index+1,i,temp);
	}
}

bool ProjectListBox::UpdateStatus(int index, Project* project)
{
	bool change = false;
	wxString newStatus;
	for(int i=0; i< this->GetColumnCount(); i++)
	{
		if(columns[i]==STATUS)
		{
			newStatus = project->GetStatusText();
			if(newStatus.Cmp(GetText(index,i))!=0)
			{
				change = true;
				this->SetItem(index,i,newStatus);
			}
		}
	}
	return change;
}


//private methods:

wxString ProjectListBox::GetAttributeString(int i, Project* project)
{
	wxString str;
	switch(i){
		case 0:
			return wxString::Format(_T("%d"),project->id);
		case 1:
			return project->path;
		case 2:
			return project->prefix;
		case 7:
			return project->GetStatusText();	//all following cases default to an empty string if file is missing
		case 3:
			if(project->status!=Project::MISSING)
				return project->modDate.FormatDate()+_T(", ")+project->modDate.FormatTime();
		case 4:
			if(project->status!=Project::MISSING)
			{
				//str = wxString::FromAscii(project->options.getFormatName(project->options.outputFormat).c_str());
				//str = str+wxT(" (.")+wxString::FromAscii(project->options.outputImageType.c_str())+wxT(")");
				str = GetLongerFormatName(project->options.outputImageType);
				str = str+wxT(" (.")+wxString::FromAscii(project->options.outputImageType.c_str())+wxT(")");
				return str;
			}
		case 5:
			if(project->status!=Project::MISSING)
				return this->projectionFormat[project->options.getProjection()];
		case 6:
			if(project->status!=Project::MISSING)
			{
				str = wxString() << project->options.getWidth();
				str = str+_T("x");
				str = str << project->options.getHeight();
				return str;
			}
		default:
			return _T("");
	}
}

wxString ProjectListBox::GetLongerFormatName(std::string str)
{
	if(str=="tif")
		return _T("TIFF");
	else if(str=="jpg")
		return _T("JPEG");
	else if(str=="png")
		return _T("PNG");
	else if(str=="exr")
		return _T("EXR");
	else
		return _T("");
}

void ProjectListBox::OnDeselect(wxListEvent &event)
{
	m_selected = -1;
}

void ProjectListBox::OnColumnWidthChange(wxListEvent &event)
{
	int col = event.GetColumn();
	wxConfigBase::Get()->Write(wxString::Format(wxT("/BatchList/ColumnWidth%d"),columns[col]), GetColumnWidth(col));
}

void ProjectListBox::OnSelect(wxListEvent &event)
{
	m_selected = ((wxListEvent)event).GetIndex();
}

const wxString ProjectListBox::columnTitle[] = {
			_T("ID"),
			_T("Project"),
			_T("Output prefix"),
			_T("Last modified"),
			_T("Output format"),
			_T("Projection"),
			_T("Size"),
			_T("Status")};

const wxString ProjectListBox::projectionFormat[] = {
			_T("RECTILINEAR"),
			_T("CYLINDRICAL"),
			_T("EQUIRECTANGULAR"),
			_T("FULL_FRAME_FISHEYE"),
			_T("STEREOGRAPHIC"),
			_T("MERCATOR"),
			_T("TRANSVERSE_MERCATOR"),
			_T("SINUSOIDAL"),
			_T("LAMBERT"),
			_T("LAMBERT_AZIMUTHAL"),
			_T("ALBERS_EQUAL_AREA_CONIC"),
			_T("MILLER_CYLINDRICAL"),
			_T("PANINI")};

const wxString ProjectListBox::fileFormat[] = {_T("JPEG"),
			_T("PNG"),
            _T("TIFF"),
            _T("TIFF_m"),
            _T("TIFF_mask"),
            _T("TIFF_multilayer"),
            _T("TIFF_multilayer_mask"),
            _T("PICT"),
            _T("PSD"),
            _T("PSD_m"),
            _T("PSD_mask"),
            _T("PAN"),
            _T("IVR"),
            _T("IVR_java"),
            _T("VRML"),
            _T("QTVR"),
            _T("HDR"),
            _T("HDR_m"),
            _T("EXR"),
            _T("EXR_m"),
            _T("FILEFORMAT_NULL")
        };

const wxString ProjectListBox::outputMode[] = {
            _T("OUTPUT_LDR"),
            _T("OUTPUT_HDR")
        };

const wxString ProjectListBox::HDRMergeType[] = {
            _T("HDRMERGE_AVERAGE"),
            _T("HDRMERGE_DEGHOST")
        };

const wxString ProjectListBox::blendingMechanism[] = {
            _T("NO_BLEND"),
            _T("PTBLENDER_BLEND"),
            _T("ENBLEND_BLEND"),
            _T("SMARTBLEND_BLEND"),
            _T("PTMASKER_BLEND")
        };

const wxString ProjectListBox::colorCorrection[] = {
            _T("NONE"),
            _T("BRIGHTNESS_COLOR"),
            _T("BRIGHTNESS"),
            _T("COLOR")
        };
