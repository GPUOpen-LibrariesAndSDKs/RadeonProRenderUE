#pragma once

#include "MaterialGraph/MaterialGraph.h"
#include "MaterialGraph/MaterialGraphSchema.h"

static void LayoutMaterial(UMaterialInterface* MaterialInterface)
{
    UMaterial* Material = MaterialInterface->GetMaterial();
    Material->EditorX = 0;
    Material->EditorY = 0;

    const int32 Height = 50;
    const int32 Width = 150;

    // layout X to make sure each input is 1 step further than output connection
    bool bContinue = true;
    while (bContinue)
    {
        bContinue = false;

        for (int32 ExpressionIndex = 0; ExpressionIndex < Material->Expressions.Num(); ++ExpressionIndex)
        {
            UMaterialExpression* Expression = Material->Expressions[ExpressionIndex];
            Expression->MaterialExpressionEditorX = FMath::Min(Expression->MaterialExpressionEditorX, -Width);

            TArray<FExpressionInput*> Inputs = Expression->GetInputs();
            for (int32 InputIndex = 0; InputIndex < Inputs.Num(); ++InputIndex)
            {
                UMaterialExpression* Input = Inputs[InputIndex]->Expression;
                if (Input != nullptr)
                {
                    if (Input->MaterialExpressionEditorX > Expression->MaterialExpressionEditorX - Width)
                    {
                        Input->MaterialExpressionEditorX = Expression->MaterialExpressionEditorX - Width;
                        bContinue = true;
                    }
                }
            }
        }
    }

    // run through each column of expressions, sort them by outputs, and layout Y
    bContinue = true;
    int32 Column = 1;
    while (bContinue)
    {
        TArray<UMaterialExpression*> ColumnExpressions;
        for (int32 ExpressionIndex = 0; ExpressionIndex < Material->Expressions.Num(); ++ExpressionIndex)
        {
            UMaterialExpression* Expression = Material->Expressions[ExpressionIndex];

            if (Expression->MaterialExpressionEditorX == -Width * Column)
            {
                Expression->MaterialExpressionEditorY = 0;
                int32 NumOutputs = 0;

                // all the connections to the material
                for (int32 MaterialPropertyIndex = 0; MaterialPropertyIndex < MP_MAX; ++MaterialPropertyIndex)
                {
                    FExpressionInput* FirstLevelExpression = Material->GetExpressionInputForProperty(EMaterialProperty(MaterialPropertyIndex));
                    if (FirstLevelExpression != nullptr && FirstLevelExpression->Expression == Expression)
                    {
                        ++NumOutputs;
                        Expression->MaterialExpressionEditorY += MaterialPropertyIndex * 20;
                    }
                }

                // all the outputs to other expressions
                for (int32 OtherExpressionIndex = 0; OtherExpressionIndex < Material->Expressions.Num(); ++OtherExpressionIndex)
                {
                    UMaterialExpression* OtherExpression = Material->Expressions[OtherExpressionIndex];
                    TArray<FExpressionInput*> Inputs = OtherExpression->GetInputs();
                    for (int32 InputIndex = 0; InputIndex < Inputs.Num(); ++InputIndex)
                    {
                        if (Inputs[InputIndex]->Expression == Expression)
                        {
                            ++NumOutputs;
                            Expression->MaterialExpressionEditorY += OtherExpression->MaterialExpressionEditorY;
                        }
                    }
                }

                if (NumOutputs > 1)
                {
                    Expression->MaterialExpressionEditorY /= NumOutputs;
                }

                ColumnExpressions.Add(Expression);
            }
        }

        struct FMaterialExpressionSorter
        {
            bool operator()(const UMaterialExpression& A, const UMaterialExpression& B) const
            {
                return (A.MaterialExpressionEditorY < B.MaterialExpressionEditorY);
            }
        };
        ColumnExpressions.Sort(FMaterialExpressionSorter());

        for (int32 ExpressionIndex = 0; ExpressionIndex < ColumnExpressions.Num(); ++ExpressionIndex)
        {
            UMaterialExpression* Expression = ColumnExpressions[ExpressionIndex];

            Expression->MaterialExpressionEditorY = ExpressionIndex * Height;
        }

        ++Column;
        bContinue = (ColumnExpressions.Num() > 0);
    }
}
