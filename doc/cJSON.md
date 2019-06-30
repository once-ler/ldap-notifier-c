### Install cJSON

```bash
mkdir build
cd build
cmake .. -DENABLE_CJSON_UTILS=On -DENABLE_CJSON_TEST=Off -DCMAKE_INSTALL_PREFIX=/usr
make
make DESTDIR=$pkgdir install
```

In ubuntu, the location is here:
```
/usr/include/cjson/cJSON.h
/usr/lib/x86_64-linux-gnu/*so
```

### Using cJSON

```c
char *out;
cJSON *root, *cars, *car;
root = cJSON_CreateObject();
cars = cJSON_CreateArray();

// add cars array to root
cJSON_AddItemToObject(root, "cars", cars);

// add 1st car to cars array
cJSON_AddItemToArray(cars, car = cJSON_CreateObject());
cJSON_AddItemToObject(car, "CarType", cJSON_CreateString("BMW"));
cJSON_AddItemToObject(car, "carID", cJSON_CreateString("bmw123"));

// add 2nd car to cars array
cJSON_AddItemToArray(cars, car = cJSON_CreateObject());
cJSON_AddItemToObject(car, "CarType", cJSON_CreateString("mercedes"));
cJSON_AddItemToObject(car, "carID", cJSON_CreateString("mercedes123"));

// print everything
out = cJSON_Print(root);
printf("%s\n", out);

// free all objects under root and root itself
cJSON_Delete(root);
```
