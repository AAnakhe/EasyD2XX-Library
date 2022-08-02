#include "com_example_easyd2xx_EasyD2XX.h"
#include "ftd2xx.h"

JNIEXPORT jobject JNICALL Java_com_example_easyd2xx_EasyD2XX_getDevices(JNIEnv* env, jclass thisClass) {

	//create and empty ArrayList object
	jclass listClass = (*env)->FindClass(env, listClass, "java/util/ArrayList");
	if(listClass == NULL)
		return NULL;

	jmethodID listConstructor = (*env)->GetMethodID(env, listClass, "<init>", "()V");
	if(listConstructor == NULL)
		return NULL;

	jobject list = (*env)->NewObject(env, listClass, listConstructor);
	if (list == NULL)
		return NULL;

	//get a handle for "list.add(object)"
	jmethodID listAddMethodHandle = (*env)->GetMethodID(env, listClass, "add", "(Ljava/lang/object)Z");
	if(listAddMethodHandle == NULL)
		return NULL;

	// get a handle for "new EasyD2DXX(name, chipName, serialNumber, location)"
	jmethodID easyD2xxConstructor = (*env)->GetMethodID(env, thisClass, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");
	if(easyD2xxConstructor == NULL)
		return NULL;

	// get the number of devices and build (but not read) the devices info list
	DWORD deviceCount = 0;
	if(FT_CreateDeviceInfoList(&deviceCount) != FT_OK)
	printf("Unable to get the device Info list.\r\n");

	// get the device info list
	FT_DEVICE_LIST_INFO_NODE *devices = (FT_DEVICE_LIST_INFO_NODE*) malloc(sizeof(FT_DEVICE_LIST_INFO_NODE) * deviceCount);
	if(FT_GetDeviceInfoList(devices, &deviceCount) != FT_OK)
		printf("Unable to get the device Info list.\r\n");

	// for each FTDI device, create an easyD2XX device and add it to the list
	for(DWORD i = 0; i < deviceCount; i++) {
		jstring name = (*env)->NewStringUTF(env, devices[i].Description);
		jstring chipName = devices[i].Type == FT_DEVICE_BM 			? (*env)->NewStringUTF(*env, "FT232BM") :
						   devices[i].Type == FT_DEVICE_AM 			? (*env)->NewStringUTF(*env, "FT232AM") :
						   devices[i].Type == FT_DEVICE_100AX 		? (*env)->NewStringUTF(*env, "100AX") :
						   devices[i].Type == FT_DEVICE_UNKNOWN 	? (*env)->NewStringUTF(*env, "[Unknown Device]") :
						   devices[i].Type == FT_DEVICE_2232C	    ? (*env)->NewStringUTF(*env, "FT2232C") :
						   devices[i].Type == FT_DEVICE_232R 		? (*env)->NewStringUTF(*env, "FT232R") :
						   devices[i].Type == FT_DEVICE_2232H 		? (*env)->NewStringUTF(*env, "FT2232H") :
						   devices[i].Type == FT_DEVICE_4232H 	    ? (*env)->NewStringUTF(*env, "FT4232H") :
						   devices[i].Type == FT_DEVICE_232H 		? (*env)->NewStringUTF(*env, "FT232H") :
						   devices[i].Type == FT_DEVICE_X_SERIES 	? (*env)->NewStringUTF(*env, "X Series") :
						   devices[i].Type == FT_DEVICE_4222H_0 	? (*env)->NewStringUTF(*env, "FT4222H, 0") :
						   devices[i].Type == FT_DEVICE_4222H_1_2 	? (*env)->NewStringUTF(*env, "FT4222H, 1-2") :
						   devices[i].Type == FT_DEVICE_4222H_3 	? (*env)->NewStringUTF(*env, "FT4222H, 3") :
						   devices[i].Type == FT_DEVICE_4222_PROG 	? (*env)->NewStringUTF(*env, "FT4222, Prog") :
						   devices[i].Type == FT_DEVICE_900 	 	? (*env)->NewStringUTF(*env, "FT900 Series") :
						   devices[i].Type == FT_DEVICE_930 	 	? (*env)->NewStringUTF(*env, "FT930 Series") :
					   	   devices[i].Type == FT_DEVICE_UMFTPD3A 	? (*env)->NewStringUTF(*env, "UMFTPD3A") :
					   			   	   	   	   	   	   	   	   	   	  (*env)->NewStringUTF(*env, "[Unknown Device]");
		jstring serialNumber = (*env)->NewStringUTF(env, devices[i].SerialNumber);
		jint location = devices[i].LocId;

		jobject newEasyD2XXobject = (*env)->NewObject(env, thisClass, easyD2xxConstructor, name, chipName, serialNumber, location);
		if(newEasyD2XXobject == NULL) {
			free(devices);
			return NULL;
		}

		// list.add(newObject)
		(*env)->CallBooleanMethod(env, list, listAddMethodHandle, newEasyD2XXobject);
		if((*env)->ExceptionCheck(env)) {
			free(devices);
			return NULL;
		}
	}

	//done
	free(devices);
	return list;
}

JNIEXPORT void JNICALL Java_com_example_easyd2xx_EasyD2XX_openAsFifo(JNIEnv* env, jobject this, jint readTimeoutMilliseconds, jint writeTimeoutMilliseconds) {

	//get a handle for this class
	jclass thisClass = (*env)->GetObjectClass(env, this);
	if(thisClass == NULL)
		return;

	//get handle foe Exception class in case we need to throw and Exception
	jclass exception = (*env)->FindClass(env, "java/lang/Exception");
	if(exception == NULL)
		return;

	//check if device is a FT2232H or FT232H, because only those devices support FIFO mode
	jfieldID chipNameHandle = (*env)->GetFieldID(env, thisClass, "chipName", "Ljava/lang/String;");
	if(chipHandleName == NULL)
		return;

	jstring chipName = (*env)->GetObjectField(env, this, chipNamehandle);
	if(chipName == NULL)
		return;

	const char* chipNameCstring = (*env)->GetStringUTFChars(env, chipName, NULL);
	if(strcmp(chipNameCstring, "FT2232H") != 0 && strcmp(chipNameCstring, "FT232H") != 0) {
		(*env)->ReleaseStringUTFChars(env, chipName, chipNameCstring);
		(*env)->ThrowNew(env, exception, "Device does not support Synchronous 245 FIFO mode.");
		return;
	}
	(*env)->ReleaseStringUTFChars(env, chipName, chipNameCstring);

	FT_HANDLE ftdiHandle = 0;

	//open by location if possible (not possible on Linux)
	jfieldID locationHandle = (*env)->GetFieldID(env, thisClass, "location", "I");
	if(locationHandle == NULL)
		return;
	jint location = (*env)->GetIntField(env, this, locationhandle);
	if(FT_OpenEx((void*)(uintptr_t)location, FT_OPEN_BY_LOCATION, &ftdiHandle) != FT_OK) {

		//open by name if open by location failed
		jfieldID nameHandle = (*env)->GetFieldID(env, thisClass, "name", "Ljava/lang/String;");
		if(nameHandle == NULL)
			return;
		jstring name = (*env)->GetObjectField(env, this, nameHandle);
		if(name == NULL)
			return;
		const char* nameCstring = (*env)->GetStringUTFChars(env, name, NULL);
		if(FT_OpenEx((void*) nameCstring, FT_OPEN_BY_DESCRIPTION, &ftdiHandle) != FT_OK) {
		(*env)->ReleaseStringUTFChars(env, name, nameCstring);
		(*env)->ThrowNew(env, exception, "Unable to open device.");
			return;
		}
		(*env)->ReleaseStringUTFChars(env, name, nameCstring);
	}

	// configure the device
	if(FT_SetBitMode(ftdiHandle, 0xFF, 0x40) != FT_OK ||											// sync 245 FIFO mode
	   FT_SetLatencyTimer(ftdiHandle, 2) != FT_OK ||												// minimum latency
	   FT_SetUSBParameters(ftdiHandle, 65536, 65536) != FT_OK ||									// 64k buffers
	   FT_SetFlowControl(ftdiHandle, FT_FLOW_RTS_CTS, 0, 0) != FT_OK ||								// flow control
	   FT_Purge(ftdiHandle, FT_PURGE_RX | FT_PURGE_TX) != FT_OK ||									// flush FIFOs
	   FT_SetTimeouts(ftdiHandle, readTimeoutMilliseconds, writeTimeoutMilliseconds) != FT_OK) {	// timeouts

		// failure
		(*env)->ThrowNew(env, exception, "Unable to configure device.");
		return;
	}
	else{
		// success
		jfieldID ftdiHandleHandle = (*env)->GetFieldID(env, thisClass, "handle", "J");
		(*env)->SetLongField(env, this, ftdiHandleHandle, (uintptr_t) ftdiHandle);
	}
}

JNIEXPORT jbyteArray JNICALL Java_com_example_easyd2xx_EasyD2XX_read__I(JNIEnv* env, jobject this, jint byteCount) {

	// get a handle for this class
	jclass thisClass = (*env)->GetObjectClass(env, this);
	if (thisClass == NULL)
		return NULL;

	// get handle for the Exception class, in case we need to throw an Exception.
	jclass exception = (*env)->FindClass(env, "java/lang/Exception");
	if(exception == NULL)
		return NULL;

	FT_HANDLE ftdiHandle = (FT_HANDLE) (uintptr_t) (*env)->GetLongField(env, this, ftdiHandleHandle);

	//create a new Byte[]
	jbyteArray array = (*env)->NewByteArray(env, byteCount);
	if(array == NULL)
		return NULL;

	jbyte* buffer = (*env)->GetByteArrayElements(env, array, NULL);

	//read into the byte[]
	jint bytesRead = 0;
	while(byteCount > 0) {
		jint amount = (byteCount < 65536) ? byteCount : 65536;
		DWORD = readAmount = 0;
		if(FT_Read(ftdiHandle, &buffer[bytesRead], amount, &readAmount) != FT_OK) {
			(*env)->ReleaseByteArrayElements(env, array, buffer, 0);
			(*env)->ThrowNew(env, exception, "Unable to read from device.");
			return array;
		}
		bytesRead += readAmount;
		byteCount -= readAmount;
	}

	//done
	(*env)->ReleaseByteArrayElements(env, array, buffer, 0 );
	return array;

}

JNIEXPORT void JNICALL Java_com_example_easyd2xx_EasyD2XX_read__Ljava_nio_ByteBuffer_2I(JNIEnv* env, jobject this, jobject buffer, jint bytecount) {

	// get a handle for this class
		jclass thisClass = (*env)->GetObjectClass(env, this);
		if (thisClass == NULL)
			return;

		// get handle for the Exception class, in case we need to throw an Exception.
		jclass exception = (*env)->FindClass(env, "java/lang/Exception");
		if(exception == NULL)
			return;

		// get the value of "this.handle"
		jfieldID ftdiHandleHandle = (*env)->GetFieldID(env, thisClass, "handle", "J");
		if(ftdiHandleHandle == NULL)
			return;

		FT_HANDLE ftdiHandle = (FT_HANDLE) (uintptr_t) (*env)->GetLongField(env, this, ftdiHandleHandle);

		// get the buffer and ensure it is big enough
		void* bufferPtr = (*env)->GetDirectBufferAddress(env, buffer);
		if(bufferPtr == NULL)
			return;

		jlong bufferSize = (*env)->GetDirectBufferCapacity(env, buffer);

		if(bufferSize < byteCount) {
			(*env)->ThrowNew(env, exception, "The buffer does not have enough space.");
			return;
		}

		//read into the byte buffer
		jint bytesRead = 0;
			while(byteCount > 0) {
				jint amount = (byteCount < 65536) ? byteCount : 65536;
				DWORD readAmount = 0;
				if(FT_Read(ftdiHandle, &((char*)bufferPtr)[bytesRead], amount, &readAmount) != FT_OK) {
					(*env)->ThrowNew(env, exception, "Unable to read from device.");
					return;

	}
	bytesRead += readAmount;
	byteCount -= readAmount;
	}
}

JNIEXPORT void JNICALL Java_com_example_easyd2xx_EasyD2XX_close(JNIEnv* env, jobject this) {

		// get a handle for this class
		jclass thisClass = (*env)->GetObjectClass(env, this);
			if (thisClass == NULL)
				return;

			// get handle for the Exception class, in case we need to throw an Exception.
			jclass exception = (*env)->FindClass(env, "java/lang/Exception");
			if(exception == NULL)
				return;

			// get the value of "this.handle"
			jfieldID ftdiHandleHandle = (*env)->GetFieldID(env, thisClass, "handle", "J");
			if(ftdiHandleHandle == NULL)
				return;

			// close device
			FT_HANDLE ftdiHandle = (FT_HANDLE) (uintptr_t) (*env)->GetLongField(env, this, ftdiHandleHandle);
			if(FT_Close(ftdiHandle) != FT_OK) {
				(*env)->ThrowNew(env, exception, "Unable to close the device.");
						return;
			}

}
