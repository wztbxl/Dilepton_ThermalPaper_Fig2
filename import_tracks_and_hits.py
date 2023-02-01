bl_info = {
    "name": "Import Tracks and Hits",
    "author": "Node Spaghetti",
    "version": (0, 0, 1),
    "blender": (2, 93, 0),
    "location": "File > Import-Export",
    "description": ("Import tracks and hits from .txt"),
    # "description": ("Import tracks and hits from .JSON"),
    "warning": "",
    "wiki_url": "",
    "tracker_url": "",
    "category": "Import-Export"}

import bpy

# ImportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ImportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator


class ImportTracksAndHits(Operator, ImportHelper):
    """This appears in the tooltip of the operator and in the generated docs"""
    bl_idname = "import.tracks_and_hits"  # important since its how bpy.ops.import_test.some_data is constructed
    bl_label = "Import Tracks & Hits"

    # ImportHelper mixin class uses this
    filename_ext = ".txt"
    # filename_ext = ".json"

    filter_glob: StringProperty(
        default="*.txt",
        # default="*.json",
        options={'HIDDEN'},
        maxlen=255,  # Max internal buffer length, longer would be clamped.
    )

#    # List of operator properties, the attributes will be assigned
#    # to the class instance from the operator settings before calling.
#    use_setting: BoolProperty(
#        name="Example Boolean",
#        description="Example Tooltip",
#        default=True,
#    )

#    type: EnumProperty(
#        name="Example Enum",
#        description="Choose between two items",
#        items=(
#            ('OPT_A', "First Option", "Description one"),
#            ('OPT_B', "Second Option", "Description two"),
#        ),
#        default='OPT_A',
#    )

    #def invoke(self, context, event):
        #wm = context.window_manager
        #return wm.invoke_props_dialog(self)
    
    def execute(self, context):
        
        import bpy, os, json
        from mathutils import Vector, Matrix, Color

        filepath = self.filepath

        # with open(filepath, 'r') as fileOb:
            # json_dict = json.load(fileOb) # trans the json file to the dictionary

        # try to read the text file
        itextFile = open(filepath, 'r')

        #for k, v in json_dict.items():
        #    print (k)
        #    for data in v:
        #        print (data, '\n')


        # taken from chaintools
        def SplineFromPoints(obCrv, splType, points): #obCrv : Cruve object in blender or is a bpy object, splType: line format(NURBS or Bezier), points: list(?) read from data files 
            dataCrv = obCrv.data
            spl = dataCrv.splines.new(splType) #add a new splines to data curve, ref https://docs.blender.org/api/current/bpy.types.CurveSplines.html#bpy.types.CurveSplines.new, return is Spline. Spline ref: https://docs.blender.org/api/current/bpy.types.Spline.html#bpy.types.Spline
            if (splType == 'BEZIER'):
                spl.bezier_points.add(len(points) - 1) # Parameters count (int in [0, inf]) – Number, Number of points to add to the spline ref:https://docs.blender.org/api/current/bpy.types.SplineBezierPoints.html#bpy.types.SplineBezierPoints.add 
                for p, bp in zip(points,spl.bezier_points): #for a, b in zip() is loop two variables, p is the element of points bp is bezier point for a curve spl.bezier_points give a Collection of points for Bezier curves only
                    bp.co = p # add the control point for the bezier pionts, ref : https://docs.blender.org/api/current/bpy.types.BezierSplinePoint.html#bpy.types.BezierSplinePoint.co
                    bp.handle_left_type  = 'AUTO' #set the handle for the added control point, p should be a collection for 3 points like (0.0, 0.0, 0.0) p is the vector of mathutils
                    bp.handle_right_type = 'AUTO'
            else: # ( (splType == 'NURBS') or (splType == 'POLY') ):
                spl.points.add(len(points) - 1)
                for p, cv in zip(points, spl.points):
                    cv.co = p.to_4d()
            return spl

        scaling_factor = 1/10

        #text reading test
        tracks = itextFile.readlines()
        name_num = 0
        name = 'track.'
        # print(tracks) # this for test
        if (tracks):
            for track in tracks:
                pointlist = list(map(float, track.split()))
                # for i in range(0,200,1) :
                # points = [ Vector((-pointlist[i*3+0]*scaling_factor, -pointlist[i*3+1]*scaling_factor, -pointlist[i*3+2]*scaling_factor) ) for i in range(0,200,1)] # inverse the coordinate of each point
                points = [ Vector((pointlist[i*3+0]*scaling_factor, pointlist[i*3+1]*scaling_factor, pointlist[i*3+2]*scaling_factor) ) for i in range(0,200,1)]
                dataCrv = bpy.data.curves.new(name + str(name_num).zfill(3), 'CURVE') #new a blender data curves 
                dataCrv.dimensions = '3D'
                obCrv = bpy.data.objects.new(name + str(name_num).zfill(3), dataCrv) # add the data curve as a object
                SplineFromPoints(obCrv, 'BEZIER', points) # points loads from the data
                bpy.context.collection.objects.link(obCrv)

                dataCrv.bevel_depth = 1
                radius = 1/10
                if len(pointlist) == 601 :
                    # radius = 1/10
                    radius = 3/10
                    if pointlist[600] == 1 :
                        obCrv.color =  1,0,0,1.0
                    if pointlist[600] == 0 :
                        obCrv.color =  0,0,1,1.0
                else : 
                    radius = 1/10
                    obCrv.color =  0,0,0,1.0
                for spl in dataCrv.splines:
                    for bp in spl.bezier_points:
                        bp.radius = radius
                        # bp.radius = radius*scaling_factor
                # obCrv.color =  0,0,0,1.0


        #Json reading method
        # tracks = json_dict.get("tracks", None) # dict.get(key[, value]) try to find the key with "tracks"
        # name_num = 0
        # name = 'track.'
        # print(tracks) # this
        # if (tracks):
        #     for track in tracks:
        #         points_dicts = track["points"]
        #         points = [ Vector((points_dict['x']*scaling_factor, points_dict['y']*scaling_factor, points_dict['z']*scaling_factor) ) for points_dict in points_dicts] # x,y,z label is the point how many points I need？
                
        #         radius = track["radius"]
        #         track_color  = track["color"]
        #         # make a curve object
        #         dataCrv = bpy.data.curves.new(name + str(name_num).zfill(3), 'CURVE') #new a blender data curves 
        #         dataCrv.dimensions = '3D'
        #         obCrv = bpy.data.objects.new(name + str(name_num).zfill(3), dataCrv) # add the data curve as a object
        #         SplineFromPoints(obCrv, 'BEZIER', points) # points loads from the data
        #         bpy.context.collection.objects.link(obCrv)
                
        #         dataCrv.bevel_depth = 1
        #         for spl in dataCrv.splines:
        #             for bp in spl.bezier_points:
        #                 bp.radius = radius*scaling_factor
                        
        #         obCrv.color =  track_color['r'], track_color['g'], track_color['b'], 1.0
                
                        
                
                name_num += 1
                

        import bmesh

        # hits = json_dict.get("hits", None)
        # name_num = 0
        # name = 'hit.'

        # if (hits):
        #     for hit in hits:
        #         point = Vector((hit['x']*scaling_factor, hit['y']*scaling_factor, hit['z']*scaling_factor) )
        #         radius = hit["radius"]
        #         hit_color = hit["color"]
                
        #         bm = bmesh.new()
                
        #         mesh = bpy.data.meshes.new(name + str(name_num).zfill(3),)
        #         obMesh = bpy.data.objects.new(name + str(name_num).zfill(3), mesh)
                
        #         bmesh.ops.create_icosphere(bm, subdivisions=2, diameter=radius*scaling_factor, matrix = Matrix.Translation(point) )
        #         bm.to_mesh(mesh)
                
        #         obMesh.color = hit_color['r'], hit_color['g'], hit_color['b'], 1.0
                
        #         bpy.context.collection.objects.link(obMesh)
                
        return {"FINISHED"}
    
# Only needed if you want to add into a dynamic menu
def menu_func_import(self, context):
    self.layout.operator(ImportTracksAndHits.bl_idname, text="Import Tracks and Hits")


def register():
    bpy.utils.register_class(ImportTracksAndHits)
    bpy.types.TOPBAR_MT_file_import.append(menu_func_import)


def unregister():
    bpy.utils.unregister_class(ImportTracksAndHits)
    bpy.types.TOPBAR_MT_file_import.remove(menu_func_import)


if __name__ == "__main__":
    register()
