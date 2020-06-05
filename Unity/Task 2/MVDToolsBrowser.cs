using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEditor;
using UnityEngine;
using UnityEngine.Events;

[Serializable] public class VoidEvent : UnityEvent { }
[Serializable] public class BoolEvent : UnityEvent<bool> { }
[Serializable] public class IntEvent : UnityEvent<int> { }
[Serializable] public class StringEvent : UnityEvent<string> { }

public class AssetInfo
{
    public string Guid;
    public string Path;
    public string Name;

    public Texture2D preview;

    public AssetInfo(string guid, string path)
    {
        Guid = guid;
        Path = path;
    }
}

public class MVDToolsBrowser
{
    public struct BrowserSettings
    {
        public string search;

        public bool toggleFilter;
        public bool toggleRefresh;
        public Vector3 mousepos;

        public GUIStyle style;
        public GUIStyle labelStyle;
        public GUIStyle labelStyleSelected;
        public GUIStyle backgroundDefault;
        public GUIStyle backgroundPreview;
    }

    public struct SizeSettings
    {
        public Vector2 scroll;

        public float browserWidth;
        public float minSpacing;
        public float widthShift;
        public float offset;
    }

    // Browser settings
    public SizeSettings sizeSettings;
    public BrowserSettings browserSettings;

    // Browser current asset
    public string selectedAsset;
    public GameObject dragObject;
    public Dictionary<string, AssetInfo> assetList;

    // Browser Events
    public UnityEvent eventUpdate;
    public StringEvent eventFilter;
    public StringEvent eventRefresh;

    public bool handleSelection;
    private Editor editorObject;
    private UnityEngine.Object selectedObject;

    public GameObject SelectedPrefab
    {
        get
        {
            if (selectedAsset != string.Empty && assetList.ContainsKey(selectedAsset))
            {
                return AssetDatabase.LoadAssetAtPath(assetList[selectedAsset].Path, typeof(UnityEngine.Object)) as GameObject;
            }

            return null;
        }
    }

    public MVDToolsBrowser(bool selection = true)
    {
        selectedAsset = string.Empty;
        assetList = new Dictionary<string, AssetInfo>();

        sizeSettings.browserWidth = 120f;
        sizeSettings.minSpacing = 15f;
        sizeSettings.widthShift = 18f;
        sizeSettings.offset = 10f;
        browserSettings.search = string.Empty;

        eventUpdate = new UnityEvent();
        eventFilter = new StringEvent();
        eventRefresh = new StringEvent();
        handleSelection = selection;
    }

    public void LoadStyles()
    {
        Texture2D texture = new Texture2D(1, 1);
        texture.SetPixel(0, 0, new Color(0.133f,0.286f,0.5f));
        texture.Apply();

        Texture2D texture2 = new Texture2D(1, 1);
        texture2.SetPixel(0, 0, new Color(0.32f, 0.32f, 0.32f));
        texture2.Apply();

        Texture2D texture3 = new Texture2D(1, 1);
        texture3.SetPixel(0, 0, new Color(0.28f, 0.28f, 0.28f));
        texture3.Apply();

        browserSettings.style = new GUIStyle();
        browserSettings.style.imagePosition = ImagePosition.ImageOnly;
        browserSettings.style.margin = new RectOffset(0, 0, 0, 0);
        browserSettings.style.padding = new RectOffset(0, 0, 0, 0);
        browserSettings.style.clipping = TextClipping.Clip;
        browserSettings.style.contentOffset = Vector2.zero;
        browserSettings.style.padding = new RectOffset(4, 4, 4, 4);
        browserSettings.style.alignment = TextAnchor.MiddleCenter;
        browserSettings.style.fixedHeight = 0;
        browserSettings.style.border = new RectOffset(4, 4, 4, 4);
        browserSettings.style.normal.background = texture2;

        browserSettings.labelStyle = new GUIStyle();
        browserSettings.labelStyle.alignment = TextAnchor.MiddleCenter;
        browserSettings.labelStyle.normal.textColor = Color.grey;

        browserSettings.labelStyleSelected = new GUIStyle("AssetLabel");
        browserSettings.labelStyleSelected.alignment = TextAnchor.MiddleCenter;
        browserSettings.labelStyleSelected.normal.textColor = Color.white;
        browserSettings.labelStyleSelected.normal.background = texture;
        //browserSettings.labelStyleSelected.fontStyle = FontStyle.Bold;

        browserSettings.backgroundDefault = new GUIStyle();
        browserSettings.backgroundDefault.normal.background = texture3;
        browserSettings.backgroundDefault.normal.background.name = "test";

        browserSettings.backgroundPreview = new GUIStyle(EditorStyles.helpBox);
        //browserSettings.backgroundPreview.normal.background = Texture2D.whiteTexture;
        //browserSettings.backgroundPreview.border = new RectOffset(2, 2, 2, 2);
    }

    public void SetSelected(string guid)
    {
        selectedAsset = guid;
        selectedObject = AssetDatabase.LoadAssetAtPath(assetList[selectedAsset].Path, typeof(UnityEngine.Object));

        if(editorObject == null || selectedObject != editorObject.target)
            editorObject = Editor.CreateEditor(selectedObject);

        if (handleSelection)
            Selection.activeObject = selectedObject;
    }

    public void Refresh(string[] guids)
    {
        FillBrowser(guids);
    }

    public void FillBrowser(AssetInfo[] assets)
    {
        assetList.Clear();

        foreach (AssetInfo asset in assets)
            assetList.Add(asset.Guid, asset);

        if (assets.Length > 0)
            SetSelected(assets[0].Guid);
    }

    public void FillBrowser(string[] guids)
    {
        assetList.Clear();

        // Loop through all guids and load the assets.
        foreach (string guid in guids)
        {
            // We loop through each asset and load it, obtain its preview (if it has one) and then i save this obj into a list
            string assetPath = AssetDatabase.GUIDToAssetPath(guid);
            UnityEngine.Object asset = AssetDatabase.LoadAssetAtPath(assetPath, typeof(UnityEngine.Object));
            Texture2D preview = AssetPreview.GetAssetPreview(asset) ?? AssetPreview.GetMiniThumbnail(asset);

            AssetInfo assetInfo = new AssetInfo(guid, assetPath);
            assetInfo.preview = preview;
            assetInfo.Name = asset.name;
            assetList.Add(guid, assetInfo);
        }
    }

    // Display function, 
    public void Display()
    {
        GUILayout.BeginHorizontal(EditorStyles.toolbar);
        {
            if (GUILayout.Toggle(browserSettings.toggleRefresh, "Refresh", EditorStyles.toolbarButton))
            {
                eventRefresh.Invoke(browserSettings.search);
            }

            if (GUILayout.Toggle(browserSettings.toggleFilter, "Filter search", EditorStyles.toolbarButton))
            {
                eventFilter.Invoke(browserSettings.search);
            }
            GUILayout.FlexibleSpace();

            browserSettings.search = EditorGUILayout.TextField(GUIContent.none, browserSettings.search, "ToolbarSeachTextField", GUILayout.Height(EditorGUIUtility.singleLineHeight));
            if (GUILayout.Button(GUIContent.none, string.IsNullOrEmpty(browserSettings.search) ? "ToolbarSeachCancelButtonEmpty" : "ToolbarSeachCancelButton"))
            {
                browserSettings.search = string.Empty;
                GUI.FocusControl("");
            }
        }
        GUILayout.EndHorizontal();

        GUILayout.BeginVertical(browserSettings.backgroundDefault);
        {
            GUILayout.Space(300);
            GUILayout.BeginHorizontal();
            GUILayout.Space(300);
            GUILayout.EndHorizontal();
        }
        GUILayout.EndVertical();

        {
            Rect trect = GUILayoutUtility.GetLastRect();
            Event current = Event.current;

            if (handleSelection)
                UpdateBrowser(trect, current);

            DisplayGrid(trect, current);
        }
    }

    // Method used to display prefabs list.
    public void DisplayGrid(Rect prefabListRect, Event current)
    {
        int xCount = Mathf.FloorToInt((prefabListRect.width - sizeSettings.minSpacing) / (sizeSettings.browserWidth + sizeSettings.minSpacing));
        int yCount = Mathf.CeilToInt(assetList.Count / (float)xCount);

        float freeWidth = ((prefabListRect.width - sizeSettings.minSpacing) - sizeSettings.offset) - (xCount * sizeSettings.browserWidth);
        float spacing = freeWidth / xCount;

        Rect contentRect = new Rect(
            prefabListRect.x, 0, prefabListRect.width - sizeSettings.widthShift, 
            yCount * (sizeSettings.browserWidth + spacing + sizeSettings.widthShift) + spacing * 2);

        sizeSettings.scroll = GUI.BeginScrollView(prefabListRect, sizeSettings.scroll, contentRect);

        //disable the scrolling when dragging prefabs
        if (current.type == EventType.DragUpdated && DragAndDrop.GetGenericData("Prefab") != null)
        {
            Debug.Log("on dragging");
            DragAndDrop.visualMode = DragAndDropVisualMode.Rejected;
            current.Use();
        }

        for (int row = 0; row < yCount; row++)
        {
            for (int x = 0; x < xCount; x++)
            {
                int i = row * xCount + x;

                if (i < assetList.Count)
                {
                    AssetInfo currentAsset = assetList.ElementAt(i).Value;

                    float elementRectX = (sizeSettings.browserWidth + spacing) * x + prefabListRect.x + sizeSettings.offset;
                    float elementRectY = (sizeSettings.browserWidth + spacing + sizeSettings.offset) * row + sizeSettings.offset;

                    Rect elementPreviewRect = new Rect(elementRectX, elementRectY, sizeSettings.browserWidth, sizeSettings.browserWidth);
                    Rect fullElementRect = new Rect(elementPreviewRect.x, elementPreviewRect.y, elementPreviewRect.width, elementPreviewRect.height + sizeSettings.offset);

                    if (fullElementRect.y <= prefabListRect.height + sizeSettings.scroll.y && fullElementRect.y + fullElementRect.height >= sizeSettings.scroll.y)
                    {
                        if (currentAsset.preview != null)
                        {
                            bool selected = selectedAsset == currentAsset.Guid ? true : false;

                            GUI.SetNextControlName(currentAsset.Guid);
                            GUI.Box(elementPreviewRect, new GUIContent(currentAsset.preview), browserSettings.style);

                            if (fullElementRect.Contains(current.mousePosition))
                                HandleEvents(current, currentAsset);

                            EditorGUI.LabelField(
                            new Rect(elementPreviewRect.x, elementPreviewRect.y + sizeSettings.browserWidth + 3, sizeSettings.browserWidth, 18),
                            new GUIContent(currentAsset.Name), selected ? browserSettings.labelStyleSelected : browserSettings.labelStyle);

                            if(selected & !editorObject.HasPreviewGUI())
                                GUI.Box(new Rect(elementPreviewRect.x, elementPreviewRect.y, elementPreviewRect.width, elementPreviewRect.height - 2), GUIContent.none);
                        }
                        else
                        {
                            GUI.Box(elementPreviewRect, new GUIContent("No Asset at: " + currentAsset.Path));
                        }
                    }
                }
            }
        }
        GUI.EndScrollView();
    }

    // Drag and drop methods
    public void UpdateBrowser(Rect prefabListRect, Event current)
    {
        if (!prefabListRect.Contains(current.mousePosition))
        {
            if (current.type == EventType.MouseDrag && Event.current.button == 1)
            {
                if (dragObject == null)
                {
                    dragObject = PrefabUtility.InstantiatePrefab(SelectedPrefab) as GameObject;
                    SceneView.onSceneGUIDelegate += UpdateMouse;
                }
            }

            if (dragObject && current.type == EventType.Ignore)
            {
                Selection.activeObject = dragObject;
                SceneView.onSceneGUIDelegate -= UpdateMouse;
                dragObject = null;
            }
        }
    }

    public void UpdateMouse(SceneView sceneView)
    {
        if(dragObject)
        {
            Vector3 mousePos = Event.current.mousePosition;
            float ppp = EditorGUIUtility.pixelsPerPoint;
            mousePos.y = SceneView.lastActiveSceneView.camera.pixelHeight - mousePos.y;// * ppp;
            mousePos.x *= ppp;

            RaycastHit hit;
            Ray ray = SceneView.lastActiveSceneView.camera.ScreenPointToRay(mousePos);
            bool castRay = Physics.Raycast(ray, out hit, float.PositiveInfinity, ~(1 << 2), QueryTriggerInteraction.Ignore);
            dragObject.transform.position = castRay ? hit.point : ray.origin + 10 * ray.direction;
        }
    }

    // Handle mouse clicks on grid elements.
    public void HandleEvents(Event current, AssetInfo currentAsset)
    {
        if (current.type == EventType.MouseDown)
        {
            GUI.FocusControl(currentAsset.Guid);
            SetSelected(currentAsset.Guid);
            eventUpdate.Invoke();
            current.Use();
        }
    }
}
