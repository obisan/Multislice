
#ifndef	 INTERFACES_FILE_H
#define	 INTERFACES_FILE_H


#ifndef AFX_DIB_H__BEDA766E_DE04_11D3_BCA6_009027AF47C4__INCLUDED_
	//	тип изображений
	#define	 TI_UNKNOWN			0
	#define	 TI_COLOR			1
	#define	 TI_BLACKWHITE		2
	#define	 TI_BINARY			3
	//	тип данный буфера для изображения
	#define	 CHANEL_RED			1
	#define	 CHANEL_GREEN		2
	#define	 CHANEL_BLUE		3
#endif
//	тип данный буфера для изображения
#define	 TDI_BYTE			0
#define	 TDI_DOUBLE			1
#define	 TDI_FLOAT			2
#define	 TDI_INT			3



//////////////////////////////////////////////////////////////////////////////
//	интерфейс Приложения

// {80C0E341-332C-11d5-8F31-009027AF47C4}
static const GUID IID_IApplication = 
{ 0x80c0e341, 0x332c, 0x11d5, { 0x8f, 0x31, 0x0, 0x90, 0x27, 0xaf, 0x47, 0xc4 } };

interface IApplication : virtual public IUnknown
{
	virtual	HRESULT __stdcall CreateDatabase(const char* pathname, REFIID iid, void** ppvObject) = NULL;
	virtual	HRESULT __stdcall OpenDatabase(const char* pathname, REFIID iid, void** ppvObject) = NULL;
	virtual	HRESULT __stdcall ShowApplication(BOOL show) = NULL;
};



//////////////////////////////////////////////////////////////////////////////
//	интерфейс для установки связи приложения с плагинами

// {067E23C5-18A8-11d5-8F31-009027AF47C4}
static const GUID IID_IPluginCtrl = 
{ 0x67e23c5, 0x18a8, 0x11d5, { 0x8f, 0x31, 0x0, 0x90, 0x27, 0xaf, 0x47, 0xc4 } };

typedef	BOOL (*LPMENUFUNCTION) (double idi, IUnknown* iface);
typedef	BOOL (*LPFUNCTERMINATE) ();

interface IPluginCtrl : virtual public IUnknown
{
	virtual DWORD	__stdcall SetMenuCommand(LPCTSTR menuItem, LPCTSTR helpString) = 0;
	virtual BOOL	__stdcall SetMenuFunction(DWORD idm, LPMENUFUNCTION func) = 0;
	virtual BOOL	__stdcall SetFormBehav(LPCTSTR name, void* rtcForm) = 0;
	virtual DWORD	__stdcall SetMenuDatabase(LPCTSTR menuItem, LPCTSTR helpString) = 0;
	virtual HWND	__stdcall GetMainWindow() = 0;
};



//////////////////////////////////////////////////////////////////////////////
//	интерфейс IDatabaseFactory

// {60E0A4E1-4FA9-11d5-8F31-009027AF47C4}
static const GUID IID_IDatabaseFactory = 
{ 0x60e0a4e1, 0x4fa9, 0x11d5, { 0x8f, 0x31, 0x0, 0x90, 0x27, 0xaf, 0x47, 0xc4 } };

interface IDatabaseFactory : virtual public IUnknown
{
};



//////////////////////////////////////////////////////////////////////////////
//	интерфейс IImageFactory

// {60E0A4E2-4FA9-11d5-8F31-009027AF47C4}
static const GUID IID_IImageFactory = 
{ 0x60e0a4e2, 0x4fa9, 0x11d5, { 0x8f, 0x31, 0x0, 0x90, 0x27, 0xaf, 0x47, 0xc4 } };

interface IImageFactory : virtual public IUnknown
{
};



//////////////////////////////////////////////////////////////////////////////
//	интерфейс Базы Данных

// {E64842E1-18AA-11d5-8F31-009027AF47C4}
static const GUID IID_IDatabase = 
{ 0xe64842e1, 0x18aa, 0x11d5, { 0x8f, 0x31, 0x0, 0x90, 0x27, 0xaf, 0x47, 0xc4 } };

interface IDatabase : virtual public IUnknown
{
	virtual	HRESULT __stdcall ShowDatabase(BOOL show) = NULL;
	virtual	HRESULT __stdcall IsDatabaseVisible() = NULL;
	virtual	HRESULT __stdcall GetDatabasePath(char* path) = NULL;
	virtual	HRESULT __stdcall GetDatabaseId(double* id) = NULL;
	virtual	HRESULT __stdcall GetImagesCount(int* numImage) = NULL;
	virtual	HRESULT __stdcall GetAssocImages(double idi, double* idAssoc, int* numAssoc) = NULL;
	virtual	HRESULT __stdcall GetImagesList(double* idImages, void* list) = NULL;
	virtual	HRESULT __stdcall CreateImage(double* idi, LPCTSTR name, int w, int h, int typeImage) = NULL;
	//	импорт (в БД) и экспорт (из БД) изображения
	virtual	HRESULT __stdcall ImportImage(const char* pathname, double* idi, BOOL link) = NULL;
	virtual	HRESULT __stdcall ExportImage(double idi, const char* pathname) = NULL;
	virtual	HRESULT __stdcall DeleteImage(double idi, UINT flags) = NULL;
	//	связь м/у названием и идент. изображения
	virtual	HRESULT __stdcall GetImageInfo(int ind, double* idi, char* name, char* path) = NULL;
	virtual	HRESULT __stdcall GetImagePath(double idi, char* path) = NULL;
	virtual	HRESULT __stdcall GetImageName(double idi, char* name) = NULL;
	//	получение или изменение атрибутов изображения
	virtual	HRESULT __stdcall GetImageAttr(double idi, char* type, double* idOrig) = NULL;
	virtual	HRESULT __stdcall SetImageAttr(double idi, const char* type, const double* idOrig) = NULL;
	virtual	HRESULT __stdcall GetRealSize(double idi, double* w, double* h, char* edizm) = NULL;
	virtual	HRESULT __stdcall SetRealSize(double idi, double w, double h, LPCTSTR edizm) = NULL;
	virtual	HRESULT __stdcall GetNameDocument(double idi, char* fname) = NULL;
	virtual	HRESULT __stdcall SetNameDocument(double idi, const char* fname) = NULL;
	//	функции для работы с документом БД
	virtual	HRESULT __stdcall OpenImage(double idi, REFIID iid, void** ppvObject) = NULL;
	virtual	HRESULT __stdcall OpenDocument(const char* docfile, REFIID iid, void** ppvObject) = NULL;
	virtual	HRESULT __stdcall UpdateRecords() = NULL;
};



//////////////////////////////////////////////////////////////////////////////
//	интерфейс изображения и его характиристик

// {CE589920-1A01-11d5-8F31-009027AF47C4}
static const GUID IID_IImageComp = 
{ 0xce589920, 0x1a01, 0x11d5, { 0x8f, 0x31, 0x0, 0x90, 0x27, 0xaf, 0x47, 0xc4 } };

interface IImageComp : virtual public IUnknown
{
	virtual	BOOL	__stdcall IsValidImage() = NULL;
	virtual	HRESULT __stdcall ShowImage(BOOL show) = NULL;
	virtual	HRESULT __stdcall IsImageVisible() = NULL;
	//	функции для получения свойств изображения (те же что у интерфейса БД)
	virtual	HRESULT __stdcall GetImageId(double* id) = NULL;
	virtual	HRESULT __stdcall GetImageName(char* name) = NULL;
	virtual	HRESULT __stdcall GetImagePath(char* path) = NULL;
	virtual	HRESULT __stdcall GetImageType(int* type) = NULL;
	virtual	HRESULT __stdcall GetImageAttr(int* w, int* h, char* type, double* idOrig) = NULL;
	virtual	HRESULT __stdcall SetImageAttr(const char* type, const double* idOrig) = NULL;
	virtual	HRESULT __stdcall GetRealSize(double* w, double* h, char* edizm) = NULL;
	virtual	HRESULT __stdcall SetRealSize(double w, double h, LPCTSTR edizm) = NULL;
	virtual	HRESULT __stdcall GetNameDocument(char* fname) = NULL;
	virtual	HRESULT __stdcall SetNameDocument(const char* fname) = NULL;
	//	функции для отрисовки изображения
	virtual	HRESULT __stdcall DrawImage(HDC hdc, const RECT* dstRect, const RECT* srcRect) = NULL;
	//	функции для получения и изменения изображения и его харак-к
	virtual	HRESULT __stdcall GetImageRGB(void* red, void* green, void* blue, int typeElem) = NULL;
	virtual	HRESULT __stdcall GetImageData(void* data, int typeElem, int chanel) = NULL;
	virtual	HRESULT __stdcall SetImageData(void* data, int typeElem, int w, int h) = NULL;
};



//////////////////////////////////////////////////////////////////////////////
//	интерфейс документа изображения

// {D2A2D181-4ACB-11d5-8F31-009027AF47C4}
static const GUID IID_IImageDocument = 
{ 0xd2a2d181, 0x4acb, 0x11d5, { 0x8f, 0x31, 0x0, 0x90, 0x27, 0xaf, 0x47, 0xc4 } };

interface IImageDocument : virtual public IUnknown
{
	virtual	HRESULT __stdcall ShowDocument(BOOL show) = NULL;
	virtual	HRESULT __stdcall IsImageVisible() = NULL;
	virtual	HRESULT __stdcall SetSourceImage(IImageComp* iimage) = NULL;
	virtual	HRESULT __stdcall InsertText(const char* text) = NULL;
	virtual	HRESULT __stdcall InsertImage(const char* pathname) = NULL;
	virtual	HRESULT __stdcall InsertTable(const char* title, int col, int row, void* tbllist) = NULL;
};



//////////////////////////////////////////////////////////////////////////////
//	интерфейс изображения и его характиристик

// {61B2BA80-1D2B-11d5-8F31-009027AF47C4}
static const GUID IID_IBehavList = 
{ 0x61b2ba80, 0x1d2b, 0x11d5, { 0x8f, 0x31, 0x0, 0x90, 0x27, 0xaf, 0x47, 0xc4 } };

interface IBehavList : virtual public IUnknown
{
	virtual	HRESULT __stdcall UpdateRecords() = NULL;
	virtual	HRESULT __stdcall GetBehavCount(int* num) = NULL;
	virtual	HRESULT __stdcall GetBehavIds(DWORD* aID, DWORD* aidg, int* types) = NULL;
	virtual	HRESULT __stdcall GetBehavGroup(DWORD idgrp, int* num, DWORD* aidbeh, int* types) = NULL;
	virtual	HRESULT __stdcall GetBehavName(DWORD idbehav, char* name) = NULL;
	virtual	HRESULT __stdcall GetBehavData(DWORD idbehav, char* name, DWORD* idgrp, int* type, char* edizm) = NULL;
	virtual	HRESULT __stdcall CreateGroup(const char* name, DWORD* idgrp) = NULL;
	virtual	HRESULT __stdcall CreateBehav(DWORD idgrp, const char* name, DWORD* idbeh) = NULL;
	virtual	HRESULT __stdcall DeleteBehav(DWORD idbeh) = NULL;
	virtual	HRESULT __stdcall ChangeBehav(DWORD idbeh, const char* name) = NULL;
	virtual	HRESULT __stdcall ChangeBehav(DWORD idbeh, const char* name, int* type, const char* edizm) = NULL;
};



//////////////////////////////////////////////////////////////////////////////
//	шаблон характиристик полученных методом секущих

// {4EEBEE41-4886-11d5-8F31-009027AF47C4}
static const GUID IID_IImageBehav = 
{ 0x4eebee41, 0x4886, 0x11d5, { 0x8f, 0x31, 0x0, 0x90, 0x27, 0xaf, 0x47, 0xc4 } };

interface IImageBehav : virtual public IUnknown
{
	virtual	HRESULT __stdcall GetBehavString(double idbehav, char* value) = NULL;
	virtual	HRESULT __stdcall GetBehavNumber(double idbehav, double* value) = NULL;
	virtual	HRESULT __stdcall SetBehavString(double idbehav, const char* value) = NULL;
	virtual	HRESULT __stdcall SetBehavNumber(double idbehav, double value) = NULL;
	virtual	HRESULT __stdcall GetBehavInfo(double idbehav, char* name, double* idgrp, int* type, char* edizm) = NULL;
};



//////////////////////////////////////////////////////////////////////////////
//	шаблон характиристик полученных методом секущих

// {C1916603-1C5F-11d5-8F31-009027AF47C4}
static const GUID IID_IBehavSec = 
{ 0xc1916603, 0x1c5f, 0x11d5, { 0x8f, 0x31, 0x0, 0x90, 0x27, 0xaf, 0x47, 0xc4 } };

interface IBehavSec : virtual public IUnknown
{
	virtual	double __stdcall GetCellMax() = NULL;				//	макс. размер ячейки
	virtual	double __stdcall GetCellMin() = NULL;				//	мин. размер ячейки
	virtual	double __stdcall GetCellMid() = NULL;				//	средний размер ячейки
	virtual	double __stdcall GetCellSko() = NULL;				//	СКО размера ячейки
	virtual	double __stdcall GetIzb() = NULL;					//	Избыточность
	virtual	double __stdcall GetAngleMax() = NULL;				//	макс. угол
	virtual	double __stdcall GetAngleMin() = NULL;				//	мин. угол
	virtual	double __stdcall GetAnisotr() = NULL;				//	анизотропия
	virtual	HRESULT __stdcall GetEdizm(char* edizm) = NULL;		//	ед. измерения
	virtual	void   __stdcall SetCellMax(double newval) = NULL;
	virtual	void   __stdcall SetCellMin(double newval) = NULL;
	virtual	void   __stdcall SetCellMid(double newval) = NULL;
	virtual	void   __stdcall SetCellSko(double newval) = NULL;
	virtual	void   __stdcall SetIzb(double newval) = NULL;
	virtual	void   __stdcall SetAngleMax(double newval) = NULL;
	virtual	void   __stdcall SetAngleMin(double newval) = NULL;
	virtual	void   __stdcall SetAnisotr(double newval) = NULL;
	virtual	HRESULT __stdcall SetEdizm(LPCTSTR edizm) = NULL;
};



//////////////////////////////////////////////////////////////////////////////
//	шаблон морфологических характеристик изображения

// {C1916600-1C5F-11d5-8F31-009027AF47C4}
static const GUID IID_IBehavMorph = 
{ 0xc1916600, 0x1c5f, 0x11d5, { 0x8f, 0x31, 0x0, 0x90, 0x27, 0xaf, 0x47, 0xc4 } };

interface IBehavMorph : virtual public IUnknown
{
	virtual	double __stdcall GetSizeMid() = NULL;			//	среднее размера
	virtual	double __stdcall GetSizeSko() = NULL;			//	СКО размера
	virtual	double __stdcall GetSizeModa() = NULL;			//	Мода размера
	virtual	double __stdcall GetSizeIzb() = NULL;			//	Избыточность размера
	virtual	double __stdcall GetAnisMid() = NULL;			//	среднее анизотропии
	virtual	double __stdcall GetAnisSko() = NULL;			//	СКО анизотропии
	virtual	double __stdcall GetAnisModa() = NULL;			//	Мода анизотропии
	virtual	double __stdcall GetAnisIzb() = NULL;			//	Избыточность анизотропии
	virtual	HRESULT __stdcall GetEdizm(char* edizm) = NULL;		//	ед. измерения
	virtual	void   __stdcall SetSizeMid(double newval) = NULL;
	virtual	void   __stdcall SetSizeSko(double newval) = NULL;
	virtual	void   __stdcall SetSizeModa(double newval) = NULL;
	virtual	void   __stdcall SetSizeIzb(double newval) = NULL;
	virtual	void   __stdcall SetAnisMid(double newval) = NULL;
	virtual	void   __stdcall SetAnisSko(double newval) = NULL;
	virtual	void   __stdcall SetAnisModa(double newval) = NULL;
	virtual	void   __stdcall SetAnisIzb(double newval) = NULL;
	virtual	HRESULT __stdcall SetEdizm(LPCTSTR edizm) = NULL;
};



//////////////////////////////////////////////////////////////////////////////
//	шаблон морфологических характеристик для бинарных изображений

// {C1916602-1C5F-11d5-8F31-009027AF47C4}
static const GUID IID_IBehavMorphbin = 
{ 0xc1916602, 0x1c5f, 0x11d5, { 0x8f, 0x31, 0x0, 0x90, 0x27, 0xaf, 0x47, 0xc4 } };

interface IBehavMorphbin : virtual public IUnknown
{
	virtual	double __stdcall GetSquare() = NULL;		//	площадь
	virtual	double __stdcall GetLength() = NULL;		//	длина
	virtual	double __stdcall GetWidth() = NULL;			//	ширина
	virtual	double __stdcall GetOrient() = NULL;		//	ориентация
	virtual	double __stdcall GetPerimetr() = NULL;		//	периметр
	virtual	double __stdcall GetEffSize() = NULL;		//	эффективный размер
	virtual	double __stdcall GetAnisotr() = NULL;		//	анизотропия
	virtual	double __stdcall GetNeighbor() = NULL;		//	ближние соседи
	virtual	HRESULT __stdcall GetEdizm(char* edizm) = NULL;		//	ед. измерения
	virtual	void   __stdcall SetSquare(double newval) = NULL;
	virtual	void   __stdcall SetLength(double newval) = NULL;
	virtual	void   __stdcall SetWidth(double newval) = NULL;
	virtual	void   __stdcall SetOrient(double newval) = NULL;
	virtual	void   __stdcall SetPerimetr(double newval) = NULL;
	virtual	void   __stdcall SetEffSize(double newval) = NULL;
	virtual	void   __stdcall SetAnisotr(double newval) = NULL;
	virtual	void   __stdcall SetNeighbor(double newval) = NULL;
	virtual	HRESULT __stdcall SetEdizm(LPCTSTR edizm) = NULL;
};



//////////////////////////////////////////////////////////////////////////////
//	шаблон КС. характеристик изображения

// {C1916601-1C5F-11d5-8F31-009027AF47C4}
static const GUID IID_IBehavKsa = 
{ 0xc1916601, 0x1c5f, 0x11d5, { 0x8f, 0x31, 0x0, 0x90, 0x27, 0xaf, 0x47, 0xc4 } };

interface IBehavKsa : virtual public IUnknown
{
	virtual	void   __stdcall SetPart2(double newval) = NULL;		//	доля второй компоненты
	virtual	void   __stdcall SetRadKorr(double newval) = NULL;		//	радиус корреляции
	virtual	void   __stdcall SetExp1(double newval) = NULL;			//	показатель эксп. 1-ой компоненты
	virtual	void   __stdcall SetExp2(double newval) = NULL;			//	показатель эксп. 2-ой компоненты
	virtual	void   __stdcall SetFreq2(double newval) = NULL;		//	частота второй компоненты
	virtual	void   __stdcall SetStohPer(double newval) = NULL;		//	стохастичность и периодичность
	virtual	void   __stdcall SetAnis(double newval) = NULL;			//	анизотропия
	virtual void   __stdcall SetErrApproc(double newval) = NULL;	//	ошибка аппроксимации
	virtual void   __stdcall SetDataIch(double* pData, int sizeArray) = NULL;
	virtual void   __stdcall SetDataIph(double* pData, int sizeArray) = NULL;
	virtual	double __stdcall GetPart2() = NULL;
	virtual	double __stdcall GetRadKorr() = NULL;
	virtual	double __stdcall GetExp1() = NULL;
	virtual	double __stdcall GetExp2() = NULL;
	virtual	double __stdcall GetFreq2() = NULL;
	virtual	double __stdcall GetStohPer() = NULL;
	virtual	double __stdcall GetAnis() = NULL;
	virtual double __stdcall GetErrApproc() = NULL;
	virtual	int	   __stdcall GetSizeIch() = NULL;	//	размер вектора ИЧХ
	virtual	int	   __stdcall GetSizeIph() = NULL;	//	размер вектора ИПХ
	virtual void   __stdcall GetDataIch(double* pData) = NULL;
	virtual void   __stdcall GetDataIph(double* pData) = NULL;
	virtual	HRESULT __stdcall GetEdizm(char* edizm) = NULL;		//	ед. измерения
	virtual	HRESULT __stdcall SetEdizm(LPCTSTR edizm) = NULL;
};



#endif	//	INTERFACES_FILE_H
