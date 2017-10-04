#ifndef INIT_ASDF_H
#define INIT_ASDF_H
void PutIAttribute(ASDF *asdf, char *attribute_name, int attribute_value);
void PutFAttribute(ASDF *asdf, char *attribute_name, float attribute_value);
void InitializeASDF (ASDF *asdf);
void InitializeStandardFrontEnd(ASDF *asdf,FILE *fp);
F_VECTOR *GsfRead(ASDF *asdf, int frame_index, char *feature_name);
F_VECTOR *ComputeFeature(ASDF *asdf, int frame_index, char *feature_name);
void GsfOpen(ASDF *asdf, char *filename);
void GsfClose(ASDF *asdf);
int GetIAttribute(ASDF *asdf, char *string);
float GetFAttribute(ASDF *asdf, char *string);
void *GetPtrAttribute(ASDF *asdf, char *string);
int select_channel;
#endif

