import pandas as pd

df = pd.read_csv("DiseaseAndSymptoms.csv")

disease_ohe = pd.get_dummies(df['Disease'],prefix='Disease').astype(int)

symptom_cols = df.columns[1:]
all_symptoms = pd.unique(df[symptom_cols].values.ravel())
all_symptoms = [s for s in all_symptoms if pd.notna(s)]

symptom_ohe = pd.DataFrame(0,index=df.index, columns=all_symptoms)

for col in symptom_cols:
    for i, val in df[col].items():
        if pd.notna(val):
            symptom_ohe.at[i,val] = 1


final_df  = pd.concat([disease_ohe, symptom_ohe], axis=1)

final_df.to_csv("DiseaseAndSymptoms_onehot.csv", index=False)